#include "scenario.h"
#include "bann_scenario.h"
#include "scenevocond.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "btmain.h" // bt_global::btmain
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h" //SkinContext

#include <QDomNode>
#include <QString>
#include <QDebug>
#include <QList>


Scenario::Scenario(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
	section_id = getTextChild(config_node, "id").toInt();
}

int Scenario::sectionId()
{
	return section_id;
}

banner *Scenario::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");

	QString what = getTextChild(item_node, "what");
	if (!what.isEmpty())
		where = what + "*" + where;

	banner *b = 0;
	switch (id)
	{
	case SCENARIO:
		b = new BannSimpleScenario(0, item_node);
		break;
	case MOD_SCENARI:
		b = new ScenarioModule(0, item_node);
		break;
	case SCENARIO_EVOLUTO:
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		ScenEvoTimeCondition *time_cond = 0;
		ScenEvoDeviceCondition *device_cond = 0;
		QString descr = getTextChild(item_node, "descr");
		QString action;
		bool enabled;

#ifdef CONFIG_BTOUCH
		bool has_next = getElement(item_node, "condDevice/value").text().toInt();
		int item_id = 0;
		QDomNode time_node = getChildWithName(item_node, "condH");

		if (!time_node.isNull() && getTextChild(time_node, "value").toInt())
			time_cond = new ScenEvoTimeCondition(item_id, time_node, has_next);

		QDomNode device_node = getChildWithName(item_node, "condDevice");
		if (!device_node.isNull() && getTextChild(device_node, "value").toInt())
			device_cond = new ScenEvoDeviceCondition(0, device_node);

		action = getElement(item_node, "action/open").text();
		enabled = getTextChild(item_node, "enable").toInt();
#else
		bool has_next = getElement(item_node, "scen/device/status").text().toInt();
		int item_id = getTextChild(item_node, "itemID").toInt();
		QDomNode time_node = getChildWithName(item_node, "scen/time");
		if (!time_node.isNull() && getTextChild(time_node, "status").toInt())
			time_cond = new ScenEvoTimeCondition(item_id, time_node, has_next);

		QDomNode device_node = getChildWithName(item_node, "scen/device");
		if (!device_node.isNull() && getTextChild(device_node, "objectID").toInt())
			device_cond = new ScenEvoDeviceCondition(item_id, device_node);

		action = getElement(item_node, "scen/action/open").text();
		enabled = getElement(item_node, "scen/status").text().toInt();
#endif
		if (time_cond)
			QObject::connect(bt_global::btmain, SIGNAL(resettimer()), time_cond, SLOT(setupTimer()));

		b = new ScenarioEvolved(item_id, descr, action, enabled, time_cond, device_cond);
	}
		break;
	case SCENARIO_SCHEDULATO:
	{
		SkinContext context(getTextChild(item_node, "cid").toInt());
		b = new ScheduledScenario(0, item_node);
		break;
	}
#ifdef CONFIG_BTOUCH
	case PPT_SCE:
		PPTSce *bann = new PPTSce(0, where, getTextChild(item_node, "cid").toInt());
		bann->setText(getTextChild(item_node, "descr"));
		b = bann;
		break;
#endif
	}

	if (b)
	{
		// TODO: these are needed for PPTSce
		b->setId(id);
		b->Draw();
	}
	return b;
}

void Scenario::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		if (banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on scenario page!", id);
		}
	}
}

