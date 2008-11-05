#include "xml_functions.h"

#include <QStringList>

QDomNode getChildWithName(QDomNode parent, QString name)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName() == name)
			return n;

		n = n.nextSibling();
	}
	return QDomNode();
}

QDomElement getElement(QDomNode root, QString path)
{
	QStringList sl = path.split('/');
	QDomNode node = root;
	while (!sl.isEmpty())
	{
		QString str = sl.first();
		if (!node.isNull())
			node = getChildWithName(node, str);
		sl.pop_front();
	}
	return node.toElement();
}

QDomNode getChildWithId(QDomNode parent, const QRegExp &node_regexp, int id)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(node_regexp))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != "id")
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text().toInt() == id)
				return n;
		}
		n = n.nextSibling();
	}
	return QDomNode();
}

