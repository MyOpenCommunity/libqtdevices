#include "skinmanager.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "main.h" // IMG_PATH

#include <QDomNode>
#include <QDebug>
#include <QFile>

#include <assert.h>


SkinManager::SkinManager(QString filename)
{
	if (QFile::exists(filename))
	{
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
		}
		else
			qWarning("SkinManager: the skin file called %s is not readable or a valid xml file",
				qPrintable(filename));
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
	assert(cid >=0 && "Invalid cid context");
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

QString SkinManager::getImage(QString name)
{
	foreach (int cid, cid_lookup_list)
		if (images[cid].contains(name))
			return IMG_PATH + images[cid][name];

	if (images[-1].contains(name))
		return IMG_PATH + images[-1][name];

	qWarning("SkinManager: No image %s found", qPrintable(name));
	return QString();
}

// The global definition of skin manager pointer
SkinManager *bt_global::skin = 0;
