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
		QFile skin_file(filename);
		
		SkinSaxParser skin_parser(style, images);
		QXmlInputSource source(&skin_file);

		QXmlSimpleReader reader;
		reader.setContentHandler(&skin_parser);

		if (!reader.parse(source))
			qWarning() << "SkinManager: Skin SAX Parsing ended with ERROR";
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


SkinSaxParser::SkinSaxParser(QString &s, QHash<int, QHash<QString, QString> > &img):
	style(s), images(img), CSS_TAG("css"), ITEM_TAG("item"), CID_TAG("cid"), IMG_TAG("img_"), COMMON_TAG("common"),
	NO_TAG("NONE_TAG"), current_section(NONE_SECTION), current_tag("")
{
}

void SkinSaxParser::handleSection()
{
	if (current_section == ITEM_SECTION)
	{
		if (current_tag == CID_TAG)
		{
			current_cid = content_buffer.toInt();
			images[current_cid] = QHash<QString, QString>();
		}
		else 
		{
			images[current_cid][current_tag.mid(4)] = content_buffer;
		}
	}
	else if (current_section == COMMON_SECTION)
	{
		images[-1][current_tag.mid(4)] = content_buffer;
	}
	else if (current_section == ROOT_SECTION)
	{
		if (current_tag == CSS_TAG)
		{
			style = content_buffer;

			//  Release unused preallocated space
			content_buffer.squeeze();
		}
	}
}

void SkinSaxParser::manageContent(ContentPhase phase)
{
	switch (phase)
	{
	case START_PHASE:
		content_buffer.clear();
		break;

	case END_PHASE:
		handleSection();
		break;
	default:
		qWarning() << "SkinSaxParser::manageContent Unnkown Phase handling";
	}
}

bool SkinSaxParser::characters(const QString &ch)
{
	// Simply collect the received stuff
	content_buffer.append(ch);
	return true;
}

bool SkinSaxParser::endDocument()
{
	current_section = NONE_SECTION;
	return true;
}

bool SkinSaxParser::endElement(const QString &namespaceURI, const QString &local_name, const QString &q_name)
{
	Q_UNUSED(q_name)
	Q_UNUSED(namespaceURI)

	current_tag = local_name;
	// Since a new element has been found we have to check if it is
	// a section identifier, if yes we have to change the section status
	if (local_name == ITEM_TAG || local_name == COMMON_TAG)
		current_section = ROOT_SECTION;
	else
	{
		// It's a leaf element end tag
		manageContent(END_PHASE);
	}

	return true;
}

bool SkinSaxParser::error(const QXmlParseException &exception)
{
	qWarning() << "SkinSaxParser::Error on line" << exception.lineNumber() << ", column"
		<< exception.columnNumber() << ":" << exception.message();
	return false;
}

bool SkinSaxParser::fatalError(const QXmlParseException &exception)
{
	qWarning() << "SkinSaxParser::Fatal error on line" << exception.lineNumber()
		<< ", column" << exception.columnNumber() << ":" << exception.message();
	return false;
}

bool SkinSaxParser::startDocument()
{
	current_section = ROOT_SECTION;
	return true;
}

bool SkinSaxParser::startElement(const QString &namespaceURI, const QString &local_name, const QString &q_name, const QXmlAttributes &atts)
{
	Q_UNUSED(q_name)
	Q_UNUSED(namespaceURI)

	current_tag = local_name;

	// Since a new element has been found we have to check if it is
	// a section identifier, if yes we have to change the section status
	if (local_name == ITEM_TAG)
	{
		current_section = ITEM_SECTION;
		if (atts.count() > 0)
		{
			// <item cid="XXX" >  v.2
			current_cid = atts.value(0).toInt();
			images[current_cid] = QHash<QString, QString>();
		}
	}
	else if (local_name == COMMON_TAG)
		current_section = COMMON_SECTION;
	else
	{
		manageContent(START_PHASE);
		if (atts.count() > 0)
			content_buffer = atts.value(0); // Assume only one tag is presente image
	}
		
	return true;
}

bool SkinSaxParser::warning(const QXmlParseException &exception)
{
	qWarning() << "SkinSaxParser::warning error on line" << exception.lineNumber()
		<< ", column" << exception.columnNumber() << ":"<< exception.message();
	return true;
}


// The global definition of skin manager pointer
SkinManager *bt_global::skin = 0;

