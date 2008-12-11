#include "lighting.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bann_lighting.h"
#include "actuators.h"

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#include <assert.h>


static QList<QString> getAddresses(QDomNode item)
{
	QList<QString> l;
	QDomNode el;
	foreach (el, getChildren(item, "element"))
		l.append(getTextChild(el, "where"));
	return l;
}


Lighting::Lighting(QDomNode config_node)
{
	loadItems(config_node);
}

void Lighting::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString where = getTextChild(item, "where");
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");
		QString img5 = IMG_PATH + getTextChild(item, "cimg5");

		QList<QString> times;
		QDomNode el;
		foreach (el, getChildren(item, "time"))
			times.append(el.toElement().text());

		banner *b;
		switch (id)
		{
		case DIMMER:
			b = new dimmer(this, where, img1, img2, img3, img4, img5);
			break;
		case ATTUAT_AUTOM:
			b = new attuatAutom(this, where, img1, img2, img3, img4);
			break;
		case GR_DIMMER:
			b = new grDimmer(this, getAddresses(item), img1, img2, img3, img4);
			break;
		case GR_ATTUAT_AUTOM:
			b = new grAttuatAutom(this, getAddresses(item), img1, img2, img3);
			break;
		case ATTUAT_AUTOM_TEMP:
			b = new attuatAutomTemp(this, where, img1, img2, img3, img4, times);
			break;
		case ATTUAT_VCT_LS:
			b = new attuatPuls(this, where, img1, img2, VCT_LS);
			break;
		case DIMMER_100:
			b = new dimmer100(this, where, img1, img2 ,img3, img4, img5, getTextChild(item,"softstart").toInt(),
					getTextChild(item,"softstop").toInt());
			break;
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
			b = new attuatAutomTempNuovoN(this, where, img1, img2, img3, img4, times);
			break;
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
			if (!times.count())
				times.append("");
			b = new attuatAutomTempNuovoF(this, where, img1, img2, img3, times.at(0));
			break;
		case GR_DIMMER100:
		{
			QList<int> sstart, sstop;
			QList<QString> addresses;

			QDomNode el;
			foreach (el, getChildren(item, "element"))
			{
				sstart.append(getTextChild(el, "softstart").toInt());
				sstop.append(getTextChild(el, "softstop").toInt());
				addresses.append(getTextChild(el, "where"));
			}

			b = new grDimmer100(this, addresses, img1, img2, img3, img4, sstart, sstop);
			break;
		}
		default:
			assert(!"Type of item not handled on lighting page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b); // TODO: deve gestire tutte le connect??
	}
}

void Lighting::initDimmer()
{
	qDebug("Lighting::initDimmer()");
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		switch(elencoBanner.at(i)->getId())
		{
		case DIMMER:
		case DIMMER_100:
		case ATTUAT_AUTOM:
		case ATTUAT_AUTOM_TEMP:
		case ATTUAT_AUTOM_TEMP_NUOVO_N:
		case ATTUAT_AUTOM_TEMP_NUOVO_F:
			elencoBanner.at(i)->inizializza(true);
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

	sottoMenu::showEvent(event);
}
