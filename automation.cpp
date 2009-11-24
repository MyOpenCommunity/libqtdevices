#include "automation.h"
#include "bann_automation.h"
#include "actuators.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bannercontent.h"
#include "main.h"

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


Automation::Automation(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int Automation::sectionId()
{
	return AUTOMAZIONE;
}

banner *Automation::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	int cid = getTextChild(item_node, "cid").toInt();
	QString where = getTextChild(item_node, "where");
	QString img1 = IMG_PATH + getTextChild(item_node, "cimg1");
	QString img2 = IMG_PATH + getTextChild(item_node, "cimg2");
	QString img3 = IMG_PATH + getTextChild(item_node, "cimg3");
	QString img4 = IMG_PATH + getTextChild(item_node, "cimg4");
	QString time = getTextChild(item_node, "time");

	banner *b = 0;
	switch (id)
	{
	case ATTUAT_AUTOM_INT_SIC:
		//b = new attuatAutomIntSic(this, where, img1, img2, img3, img4);
		b = new SecureInterblockedActuator(this, item_node);
		break;
	case ATTUAT_AUTOM_INT:
		//b = new attuatAutomInt(this, where, img1, img2, img3, img4);
		b = new InterblockedActuator(this, item_node);
		break;
	case ATTUAT_AUTOM:
		// DELETE
		//b = new attuatAutom(this, where, img1, img2, img3, img4);
		b = new SingleActuator(this, item_node, where);
		break;
	case ATTUAT_VCT_SERR:
		//b = new attuatPuls(this, where, img1, img2, VCT_SERR);
		b = new ButtonActuator(this, item_node, VCT_SERR);
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
		b = new InterblockedActuatorGroup(this, item_node);
		break;
	}
	case AUTOM_CANC_ATTUAT_ILL:
		//b = new automCancAttuatIll(this, where, img1, img2, time);
		b = new GateLightingActuator(this, item_node);
		break;
	case AUTOM_CANC_ATTUAT_VC:
		//b = new automCancAttuatVC(this, where, img1, img2);
		b = new GateEntryphoneActuator(this, item_node);
		break;
	case ATTUAT_AUTOM_PULS:
		//b = new attuatPuls(this, where, img1, img2, AUTOMAZ);
		b = new ButtonActuator(this, item_node, AUTOMAZ);
		break;
	case PPT_STAT:
		b = new PPTStat(this, where, cid);
		break;
	}

	if (b)
	{
		//b->setText(getTextChild(item_node, "descr"));
		b->setId(id);
		//b->Draw();
	}
	return b;
}

void Automation::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
			page_content->appendBanner(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on automation page!", id);
		}
	}
}

