#include "automation.h"
#include "bann_automation.h"
#include "actuators.h"
#include "xml_functions.h" // getChildren, getTextChild

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>

#include <assert.h>


Automation::Automation(const QDomNode &config_node)
{
	loadItems(config_node);
}

void Automation::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString where = getTextChild(item, "where");
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");
		QString time = getTextChild(item, "time");

		banner *b;
		switch (id)
		{
		case ATTUAT_AUTOM_INT_SIC:
			b = new attuatAutomIntSic(this, where, img1, img2, img3, img4);
			break;
		case ATTUAT_AUTOM_INT:
			b = new attuatAutomInt(this, where, img1, img2, img3, img4);
			break;
		case ATTUAT_AUTOM:
			b = new attuatAutom(this, where, img1, img2, img3, img4);
			break;
		case ATTUAT_VCT_SERR:
			b = new attuatPuls(this, where, img1, img2, VCT_SERR);
			break;
		case GR_ATTUAT_INT:
		{
			QList<QString> addresses;
			foreach (const QDomNode &el, getChildren(item, "element"))
				addresses.append(getTextChild(el, "where"));

			b = new grAttuatInt(this, addresses, img1, img2, img3);
			break;
		}
		case AUTOM_CANC_ATTUAT_ILL:
			b = new automCancAttuatIll(this, where, img1, img2, time);
			break;
		case AUTOM_CANC_ATTUAT_VC:
			b = new automCancAttuatVC(this, where, img1, img2);
			break;
		case ATTUAT_AUTOM_PULS:
			b = new attuatPuls(this, where, img1, img2, AUTOMAZ);
			break;
		default:
			assert(!"Type of item not handled on automation page!");
		}

		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b); // TODO: deve gestire tutte le connect??
	}
}
