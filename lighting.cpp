#include "lighting.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bann_lighting.h"
#include "actuators.h"
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h" // SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


static QList<QString> getAddresses(QDomNode item, QList<int> *start_values = 0, QList<int> *stop_values = 0)
{
	QList<QString> l;
	foreach (const QDomNode &el, getChildren(item, "element"))
	{
		l.append(getTextChild(el, "where"));
		if (start_values)
			start_values->append(getTextChild(el, "softstart").toInt());
		if (stop_values)
			stop_values->append(getTextChild(el, "softstop").toInt());
	}
	return l;
}


Lighting::Lighting(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int Lighting::sectionId()
{
	return ILLUMINAZIONE;
}

banner *Lighting::getBanner(const QDomNode &item_node)
{
	SkinContext ctx(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");
	QString descr = getTextChild(item_node, "descr");

	banner *b = 0;
	switch (id)
	{
	case DIMMER:
		b = new Dimmer(descr, where);
		break;
	case ATTUAT_AUTOM:
		b = new SingleActuator(0, item_node, where);
		break;
	case GR_DIMMER:
		b = new DimmerGroup(getAddresses(item_node), descr);
		break;
	case GR_ATTUAT_AUTOM:
		b = new LightGroup(getAddresses(item_node), descr);
		break;
	case ATTUAT_AUTOM_TEMP:
		b = new TempLight(0, item_node);
		break;
	case ATTUAT_VCT_LS:
		b = new ButtonActuator(0, item_node, VCT_LS);
		break;
	case DIMMER_100:
	{
		int start = getTextChild(item_node, "softstart").toInt();
		int stop = getTextChild(item_node, "softstop").toInt();
		b = new Dimmer100(descr, where, start, stop);
	}
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_N:
		b = new TempLightVariable(0, item_node);
		break;
	case GR_DIMMER100:
	{
		QList<int> start, stop;
		QList<QString> addresses = getAddresses(item_node, &start, &stop);
		b = new Dimmer100Group(addresses, start, stop, descr);
	}
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_F:
		b = new TempLightFixed(0, item_node);
		break;
	}

	if (b)
		b->setId(id);
	return b;
}

void Lighting::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
			page_content->appendBanner(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on lighting page!", id);
		}
	}
}

void Lighting::inizializza()
{
	initDimmer();
}

void Lighting::initDimmer()
{
	qDebug("Lighting::initDimmer()");
	for (int i = 0; i < page_content->bannerCount(); ++i)
	{
		banner *b = page_content->getBanner(i);
		switch (b->getId())
		{
		case DIMMER:
		case DIMMER_100:
		case ATTUAT_AUTOM:
		case ATTUAT_AUTOM_TEMP:
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
			b->inizializza(true);
			break;
		default:
			break;
		}
	}
}

