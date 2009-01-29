#include "xml_functions.h"

#include <QStringList>


QDomNode getChildWithName(const QDomNode &parent, const QString &name)
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

QDomElement getElement(const QDomNode &root, const QString &path)
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

QDomNode getChildWithId(const QDomNode &parent, const QRegExp &node_regexp, int id)
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

QList<QDomNode> getChildren(const QDomNode &parent, const QString &name)
{
	QList<QDomNode> l;
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().startsWith(name))
			l.append(n);

		n = n.nextSibling();
	}
	return l;
}

QString getTextChild(const QDomNode &parent, const QString &name)
{
	QDomNode n = getChildWithName(parent, name);
	if (n.isNull())
		return QString();
	return n.toElement().text();
}
