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


#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QList>
#include <QHash>
#include <QString>
#include <QXmlDefaultHandler>



// An utility function that return the image associated with the tagname if
// the tagname is not null (otherwise, it returns null).
QString tagToImage(const QString &tagname);


/*!
	\ingroup Core
	\brief %Loads and get stylesheet and images path defined in a skin xml file.

	To get images path, you often should set (adding or removing cid) the
	\em explicit context using addToContext() / removeFromContext() or the helper
	class SkinContext before getting them. An \em implicit context, which can be
	used to retrieve common images, is always set.
	You can also check if a tagname is defined using the exists() method, and
	save/restore the current context using getCidState() and setCidState().

	For the stylesheet the method getStyle() is provided to get the string that
	contains the global style to apply.

	This class is designed to use as a global object, through the bt_global
	namespace.
*/
class SkinManager
{
public:
	/*!
		Synonym for QList<int>
	*/
	typedef QList<int> CidState;

	/*!
		\brief %Loads the stylesheet and tagnames from the skin configuration file.
	*/
	SkinManager(QString filename);

	/*!
		\brief Returns the stylesheet reads from the skin xml file.
	*/
	QString getStyle();


	/*!
		\brief Add a cid to the skin context.

		Most of the time, you have no need to use this function directly, because
		you can use the wrapper SkinContext.
	*/
	void addToContext(int cid);

	/*!
		\brief Remove a cid to the skin context.

		\sa addToContext
	*/
	void removeFromContext();

	/*!
		\brief Check if an explicit context is set.
	*/
	bool hasContext();

	/*!
		\brief Returns the full image path.

		\a name is the tagname used in the code and also specified in the skin xml
		file withouth the prefix 'img_'. If \a name is not found an empty string is
		returned.

		All the contexts set through addToContext() are searched from the last inserted
		to the first one, so if a tagname is found in more contexts the image path
		of the last context is returned.
	*/
	QString getImage(QString name);

	/*!
		\brief Check if a \a name is defined in the skin xml file.
	*/
	bool exists(QString name);

	/*!
		\brief Return the whole skin context.
	*/
	CidState getCidState();

	/*!
		\brief Set the whole skin context.
	*/
	void setCidState(const CidState &state);

private:
	QString style;
	QList<int> cid_lookup_list;
	QHash<int, QHash<QString, QString> > images;
};


/*!
	\ingroup Core
	\brief An helper class for SkinManager.

	A simply wrapper around the SkinManager::addToContext() and
	SkinManager::removeFromContext(), useful to avoid the explicit remove calls
	from the context.
*/
class SkinContext
{
public:
	/*!
		\brief Create the object and add the \a cid to the context.
	*/
	SkinContext(int cid);

	/*!
		\brief Destroy the object and remove the previously added cid from the context.
	*/
	~SkinContext();
};


/*!
	\ingroup Core
	\brief a Sax parser version of Skin Xml Files.

	A simple Sax handler used to parse XML Skin files according to the QXmlSimpleReader class.
*/
class SkinSaxParser : public QXmlDefaultHandler
{
public:
	SkinSaxParser();

	// Overwritten QXmlDefaultHandler services to provide the required specific handling
	bool characters(const QString &ch);
	bool endDocument();
	bool endElement(const QString &namespaceURI, const QString &local_name, const QString &q_name);
	bool error(const QXmlParseException &exception);
	bool fatalError(const QXmlParseException &exception);
	bool startDocument();
	bool startElement(const QString &namespaceURI, const QString &local_name, const QString &q_name, const QXmlAttributes &atts);
	bool warning(const QXmlParseException &exception);

	QString getStyle() { return style; }
	QHash<int, QHash<QString, QString> > getImages() { return images; }

private:
	enum ContentPhase
	{
		START_PHASE = 0,
		END_PHASE = 1
	};

	void manageContent(ContentPhase phase);
	void handleSection();


private:
	// return values from the parsing
	QString style;
	QHash<int, QHash<QString, QString> > images;

	// Expected const document tag
	const char* const CSS_TAG;
	const char* const ITEM_TAG;
	const char* const CID_TAG;
	const char* const IMG_TAG;
	const char* const COMMON_TAG;
	const char* const NO_TAG;

	enum DocumentSection
	{
		NONE_SECTION = 0,
		ROOT_SECTION = 1,
		ITEM_SECTION = 2,
		COMMON_SECTION = 3
	};

	int current_cid;

	// A two level status (document_section/current_tag) is enough to define the processing context
	DocumentSection current_section;
	QString current_tag;

	QString content_buffer;
};



namespace bt_global { extern SkinManager *skin; }


#endif // SKINMANAGER_H
