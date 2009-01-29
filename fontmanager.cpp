#include "fontmanager.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <QString>
#include <QDebug>
#include <QFile>


namespace
{
	// Return the int value of an item that is actually an unsigned, so the value
	// -1 means that the child is not found.
	inline int getIntChild(const QDomNode &config_node, QString tagname)
	{
		QString text = getTextChild(config_node, tagname);
		if (text.isNull())
			return -1;

		return text.toInt();
	}
}

// FontManager is used as global object, but depends on the language var set in
// configuration file. So, the configuration must be read before loading fonts.
// To ensure that, the costructor is empty, so it doesn't matter if the global
// configuration object is built before or after the global font object.
// The loading of fonts is done through loadFonts method by btmain, after the
// reading of configuration.

FontManager::FontManager()
{
}

void FontManager::loadFonts(QString font_file)
{
	qDebug("Font file: %s", qPrintable(font_file));
	if (QFile::exists(font_file))
	{
		QFile fh(font_file);
		QDomDocument qdom;
		if (qdom.setContent(&fh))
		{
			foreach (const QDomNode &font_node, getChildren(qdom.documentElement(), "font"))
			{
				// Required fields for a font are: type, family, size, weight
				// The field descr is used for debugging and to make the xml
				// conf file more readable.
				int type = getIntChild(font_node, "type");
				QString family = getTextChild(font_node, "family");
				int size = getIntChild(font_node, "size");
				int weight = getIntChild(font_node, "weight");
				if (type == -1 || size == -1 || weight == -1)
				{
					qWarning("Missing some fields for font \"%s\" [%d]", qPrintable(getTextChild(font_node, "descr")), type);
					continue;
				}
				QFont f;
				f.setFamily(family);
				f.setPointSize(size);
				f.setWeight(weight);
				font_list[static_cast<Type>(type)] = f;
			}
		}
	}
}

const QFont& FontManager::get(Type t)
{
	if (!font_list.contains(t))
		qFatal("Font of type %d not load!", t);

	return font_list[t];
}


// The global definition of font manager
FontManager bt_global::font;

