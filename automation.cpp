#include "automation.h"
#include "bann_automation.h"
#include "actuators.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "content_widget.h" // content_widget
#include "main.h"

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


Automation::Automation(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

void Automation::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		int cid = getTextChild(item, "cid").toInt();
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
			//b = new attuatAutomIntSic(this, where, img1, img2, img3, img4);
			b = new SecureInterblockedActuator(this, item);
			break;
		case ATTUAT_AUTOM_INT:
			//b = new attuatAutomInt(this, where, img1, img2, img3, img4);
			b = new InterblockedActuator(this, item);
			break;
		case ATTUAT_AUTOM:
			// DELETE
			//b = new attuatAutom(this, where, img1, img2, img3, img4);
			b = new SingleActuator(this, item, where);
			break;
		case ATTUAT_VCT_SERR:
			b = new attuatPuls(this, where, img1, img2, VCT_SERR);
			break;
		case GR_ATTUAT_INT:
		{
			/*
			//DELETE
			QList<QString> addresses;
			foreach (const QDomNode &el, getChildren(item, "element"))
				addresses.append(getTextChild(el, "where"));

			b = new grAttuatInt(this, addresses, img1, img2, img3);
			*/
			b = new InterblockedActuatorGroup(this, item);
			break;
		}
		case AUTOM_CANC_ATTUAT_ILL:
			//b = new automCancAttuatIll(this, where, img1, img2, time);
			b = new GateLightingActuator(this, item);
			break;
		case AUTOM_CANC_ATTUAT_VC:
			//b = new automCancAttuatVC(this, where, img1, img2);
			b = new GateEntryphoneActuator(this, item);
			break;
		case ATTUAT_AUTOM_PULS:
			b = new attuatPuls(this, where, img1, img2, AUTOMAZ);
			break;
		case PPT_STAT:
			b = new PPTStat(this, where, cid);
			break;
		default:
			qFatal("Type of item not handled on automation page!");
		}

		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		b->Draw();
		content_widget->appendBanner(b);
	}
}
