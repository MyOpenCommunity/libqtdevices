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
	QString descr = getTextChild(item_node, "descr");

	banner *b = 0;
	switch (id)
	{
	case ATTUAT_AUTOM_INT_SIC:
		b = new SecureInterblockedActuator(0, item_node);
		break;
	case ATTUAT_AUTOM_INT:
		b = new InterblockedActuator(0, item_node);
		break;
	case ATTUAT_AUTOM:
		b = new SingleActuator(descr, where);
		break;
	case ATTUAT_VCT_SERR:
		b = new ButtonActuator(descr, where, VCT_SERR);
		break;
	case GR_ATTUAT_INT:
		b = new InterblockedActuatorGroup(0, item_node);
		break;
	case AUTOM_CANC_ATTUAT_ILL:
		b = new GateLightingActuator(0, item_node);
		break;
	case AUTOM_CANC_ATTUAT_VC:
		b = new GateEntryphoneActuator(0, item_node);
		break;
	case ATTUAT_AUTOM_PULS:
		b = new ButtonActuator(descr, where, AUTOMAZ);
		break;
	case PPT_STAT:
		b = new PPTStat(0, where, cid);
		break;
	}

	if (b)
		b->setId(id);

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

