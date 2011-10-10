/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "skinmanager.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "main.h" // IMG_PATH

#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QXmlSimpleReader>
#include <QXmlInputSource>


QString tagToImage(const QString &tagname)
{
	if (tagname.isNull())
		return QString();

	return bt_global::skin->getImage(tagname);
}


SkinManager::SkinManager(QString filename)
{
	if (QFile::exists(filename))
	{
		/*
		qDebug() << "******* Start Skin DOM Parser on: "<< filename;
		QFile file(filename);
		QDomDocument qdom;
		if (qdom.setContent(&file))
		{
			QDomNode root = qdom.documentElement();
			style = getTextChild(root, "css");
			foreach (const QDomNode &item, getChildren(root, "item"))
			{
				int cid = getTextChild(item, "cid").toInt();
				images[cid] = QHash<QString, QString>();
				foreach (const QDomNode &img, getChildren(item, "img_"))
				{
					QDomElement el = img.toElement();
					images[cid][el.tagName().mid(4)] = el.text();
				}
			}
			QDomNode common = getChildWithName(root, "common");
			foreach (const QDomNode &img, getChildren(common, "img_"))
			{
				QDomElement el = img.toElement();
				images[-1][el.tagName().mid(4)] = el.text(); // we use -1 as special case
			}
			
			qDebug() << "******* Skin DOM Parsing ended with success";
		}
		else
			qWarning("SkinManager: the skin file called %s is not readable or a valid xml file",
				qPrintable(filename));
		*/
		//	MC20111005	testing Sax Parser
		
		QFile skinFile(filename);
		
		qDebug() << "******* Start Skin SAX Parser on: "<< skinFile.fileName();
		
		SkinSaxParser    	skinParser(style, images);
		QXmlSimpleReader 	reader;
		QXmlInputSource 	source(&skinFile);
		
		//source.reset();
		
		reader.setContentHandler(&skinParser);
		

		if(reader.parse(source))		
			qDebug() << "******* Skin SAX Parsing ended with success";
		else
			qWarning() << "******* Skin SAX Parsing ended with ERROR";
		
	}
	else
		qWarning("SkinManager: no skin file called %s", qPrintable(filename));
}

QString SkinManager::getStyle()
{
	return style;
}

void SkinManager::addToContext(int cid)
{
	Q_ASSERT_X(cid >=0, "SkinManager::addToContext", "Invalid cid context");
	cid_lookup_list.append(cid);
}

void SkinManager::removeFromContext()
{
	cid_lookup_list.removeLast();
}

bool SkinManager::hasContext()
{
	return !cid_lookup_list.isEmpty();
}

bool SkinManager::exists(QString name)
{
	for (int i = cid_lookup_list.size() - 1; i >= 0; --i)
	{
		int cid = cid_lookup_list[i];
		if (images[cid].contains(name))
			return true;
	}

	if (images[-1].contains(name))
		return true;

	return false;
}

QString SkinManager::getImage(QString name)
{
	for (int i = cid_lookup_list.size() - 1; i >= 0; --i)
	{
		int cid = cid_lookup_list[i];
		if (images[cid].contains(name))
			return IMG_PATH + images[cid][name];
	}

	if (images[-1].contains(name))
		return IMG_PATH + images[-1][name];

	qWarning() << "SkinManager: No image" << name << "found, current context: " << cid_lookup_list;
	return QString("");
}

SkinManager::CidState SkinManager::getCidState()
{
	return cid_lookup_list;
}

void SkinManager::setCidState(const CidState &state)
{
	cid_lookup_list = state;
}


SkinContext::SkinContext(int cid)
{
	Q_ASSERT_X(bt_global::skin, "SkinContext::SkinContext", "SkinManager not yet built!");
	bt_global::skin->addToContext(cid);
}

SkinContext::~SkinContext()
{
	bt_global::skin->removeFromContext();
}





/* 
 * Skin Sax parser implementation
 */
SkinSaxParser::SkinSaxParser(QString& theStyle, QHash<int, QHash<QString, QString> >& theImages):
m_style(theStyle), m_images(theImages), CSS_TAG("css"), ITEM_TAG("item"), CID_TAG("cid"), IMG_TAG("img_"), COMMON_TAG("common"), 
NO_TAG("NONE_TAG"), m_curDocSec(none_sec), m_curTag("")
{
	//	Set a reservation to limit the expensive grow operation
	//  afterwards could be sqeezed to the size of css stuff
	//	that seems to be the biggest
	
	//m_contentBuf.reserve(1500);	
	//m_curTag.reserve(25);
	
	//m_images.reserve(150);
}


SkinSaxParser::~SkinSaxParser()	{
	;
}


void SkinSaxParser::handleSection(void)	{
	static int curCid=-1;
	
	//	Handle Item section content
	if(m_curDocSec == item_sec)	
	{
		if(m_curTag == CID_TAG)	
		{
			curCid = m_contentBuf.toInt();
			m_images[curCid] = QHash<QString, QString>();
		}
		else 
		{
				m_images[curCid][m_curTag.mid(4)] = m_contentBuf;
		}
	}
	//	Handle common section content
	else if(m_curDocSec == comm_sec)	
	{
			m_images[-1][m_curTag.mid(4)] = m_contentBuf;
	}
	//	Handle css content
	else if(m_curDocSec == root_sec)
	{
		if(m_curTag == CSS_TAG)	{
			m_style = m_contentBuf;
			
			//  Release unused preallocated space
			m_contentBuf.squeeze();
		}
	}
}



void SkinSaxParser::manageContent(MngCntPhase thePhase)	{
	
	switch (thePhase)	{
		case startPhs:
				m_contentBuf.clear();
			break;
			
		case endPhs:
				handleSection();
			break;
		
		default:
			qWarning() << "SkinSaxParser::manageContent Unnkown Phase handling";
	}
}


bool 	SkinSaxParser::characters ( const QString & ch )	{
	//qDebug() << "******* SkinSaxParser::characters: "<<ch;
	
	//	Simply collect the received stuff
	m_contentBuf.append(ch);
	
	return true;
}


bool 	SkinSaxParser::endDocument ()	{
	//qDebug() << "******* SkinSaxParser::endDocument";

	//	set the current document section to the no_section level
	m_curDocSec = none_sec;	
	
	return true;
}


bool 	SkinSaxParser::endElement ( const QString & namespaceURI, const QString & localName, const QString & qName )	{
	//qDebug() << "******* SkinSaxParser::endElement: " << localName;
	
	m_curTag = localName;
	
	//	Since a new element has been found we have to check if it is
	//	a section identifier, if yes we have to change the section status
	if( (localName == ITEM_TAG) ||
		(localName == COMMON_TAG)	
	  )
		m_curDocSec = root_sec;
	else	{
		//	It's a leaf element end tag 
		manageContent(endPhs);
	}	

	return true;
}


bool 	SkinSaxParser::error ( const QXmlParseException & exception )	{
	
	qWarning() << "SkinSaxParser::Error on line" << exception.lineNumber()<< ", column" << exception.columnNumber() << ":"<< exception.message();

	return false;	
}


bool 	SkinSaxParser::fatalError ( const QXmlParseException & exception )	{
	
	qWarning() << "SkinSaxParser::Fatal error on line" << exception.lineNumber()<< ", column" << exception.columnNumber() << ":"<< exception.message();

	return false;
}


bool 	SkinSaxParser::startDocument ()	{
	//qDebug() << "******* SkinSaxParser::startDocument";
	
	//	set the current document section to the root
	m_curDocSec = root_sec;
	
	return true;
}


bool 	SkinSaxParser::startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )	{
	//qDebug() << "******* SkinSaxParser::startElement: "<< localName;
	
	m_curTag = localName;
	
	//	Since a new element has been found we have to check if it is
	//	a section identifier, if yes we have to change the section status
	if(localName == ITEM_TAG)
		m_curDocSec = item_sec;
	else if(localName == COMMON_TAG)
		m_curDocSec = comm_sec;
	else	{
		//	It's a leaf element start tag 
		manageContent(startPhs);
	}
		
	return true;
}


bool 	SkinSaxParser::warning ( const QXmlParseException & exception )	{
	
	qWarning() << "SkinSaxParser::warning error on line" << exception.lineNumber()<< ", column" << exception.columnNumber() << ":"<< exception.message();
	
	return true;
}













// The global definition of skin manager pointer
SkinManager *bt_global::skin = 0;

