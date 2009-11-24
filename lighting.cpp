#include "lighting.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bann_lighting.h"
#include "actuators.h"
#include "bannercontent.h"
#include "main.h"

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


static QList<QString> getAddresses(QDomNode item)
{
	QList<QString> l;
	foreach (const QDomNode &el, getChildren(item, "element"))
		l.append(getTextChild(el, "where"));
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
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");
	QString img1 = IMG_PATH + getTextChild(item_node, "cimg1");
	QString img2 = IMG_PATH + getTextChild(item_node, "cimg2");
	/*
	DELETE:
	QString img3 = IMG_PATH + getTextChild(item_node, "cimg3");
	QString img4 = IMG_PATH + getTextChild(item_node, "cimg4");
	QString img5 = IMG_PATH + getTextChild(item_node, "cimg5");

	QList<QString> times;
	foreach (const QDomNode &el, getChildren(item_node, "time"))
		times.append(el.toElement().text());
	*/

	banner *b = 0;
	switch (id)
	{
	case DIMMER:
		// DELETE: remove these comments when the classes are gone
		//b = new dimmer(this, where, img1, img2, img3, img4, img5);
		b = new DimmerNew(this, item_node, where);
		break;
	case ATTUAT_AUTOM:
		b = new SingleActuator(this, item_node, where);
		break;
	case GR_DIMMER:
		// DELETE:
		//b = new grDimmer(this, getAddresses(item_node), img1, img2, img3, img4);
		b = new DimmerGroup(this, item_node, getAddresses(item_node));
		break;
	case GR_ATTUAT_AUTOM:
		b = new LightGroup(this, item_node, getAddresses(item_node));
		break;
	case ATTUAT_AUTOM_TEMP:
		//DELETE
		//b = new attuatAutomTemp(this, where, img1, img2, img3, img4, times);
		b = new TempLight(this, item_node);
		break;
	case ATTUAT_VCT_LS:
		//DELETE
		//b = new attuatPuls(this, where, img1, img2, VCT_LS);
		b = new ButtonActuator(this, item_node, VCT_LS);
		break;
	case DIMMER_100:
		// DELETE
		//b = new dimmer100(this, where, img1, img2 ,img3, img4, img5, getTextChild(item,"softstart").toInt(),
		//		getTextChild(item_node,"softstop").toInt());
		b = new Dimmer100New(this, item_node);
		break;
	case ATTUAT_AUTOM_TEMP_NUOVO_N:
		// DELETE
		//b = new attuatAutomTempNuovoN(this, where, img1, img2, img3, img4, times);
		b = new TempLightVariable(this, item_node);
		break;
	case GR_DIMMER100:
	{
		b = new Dimmer100Group(this, item_node);
		// DELETE
		/*
		QList<int> sstart, sstop;
		QList<QString> addresses;

		foreach (const QDomNode &el, getChildren(item_node, "element"))
		{
			sstart.append(getTextChild(el, "softstart").toInt());
			sstop.append(getTextChild(el, "softstop").toInt());
			addresses.append(getTextChild(el, "where"));
		}

		b = new grDimmer100(this, addresses, img1, img2, img3, img4, sstart, sstop);
		*/
		break;
	}
	case ATTUAT_AUTOM_TEMP_NUOVO_F:
		/*
		if (!times.count())
			times.append("");
		b = new attuatAutomTempNuovoF(this, where, img1, img2, img3, times.at(0));
		*/
		b = new TempLightFixed(this, item_node);
		break;
	}

	if (b)
	{
		//DELETE
		//b->setText(getTextChild(item, "descr"));
		b->setId(id);
		//b->Draw();
	}
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

void Lighting::showEvent(QShowEvent *event)
{
	qDebug() << "Lighting::showEvent()";
	initDimmer();

	Page::showEvent(event);
}
