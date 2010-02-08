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

namespace
{
#ifdef CONFIG_BTOUCH
	QList<ScenEvoCondition*> loadConditions(const QDomNode &config_node)
	{
		// NOTE: the ownership of ScenEvoCondition objects is taken by the object that
		// store the list of conditions.
		// TODO: we can have at maximum 1 condH and 1 condDevice, remove lists
		bool has_next = getElement(config_node, "condDevice/value").text().toInt();
		QList<ScenEvoCondition*> l;
		foreach (const QDomNode &cond, getChildren(config_node, "condH"))
		{
			if (getTextChild(cond, "value").toInt())
			{
				ScenEvoTimeCondition *c = new ScenEvoTimeCondition(0, cond, has_next);
				QObject::connect(bt_global::btmain, SIGNAL(resettimer()), c, SLOT(setupTimer()));
				l.append(c);
			}
		}

		foreach (const QDomNode &cond, getChildren(config_node, "condDevice"))
		{
			if (getTextChild(cond, "value").toInt())
			{
				ScenEvoDeviceCondition *c = new ScenEvoDeviceCondition(0, cond);
				l.append(c);
			}
		}
		return l;
	}
#else
	QList<ScenEvoCondition*> loadConditions(const QDomNode &config_node)
	{
		int item_id = getTextChild(config_node, "itemID").toInt();

		// NOTE: the ownership of ScenEvoCondition objects is taken by the object that
		// store the list of conditions.
		// TODO: we can have at maximum 1 condH and 1 condDevice, remove lists
		bool has_next = getElement(config_node, "scen/device/status").text().toInt();
		QList<ScenEvoCondition*> l;
		// parse time condition
		QDomNode cond = getElement(config_node, "scen/time");
		if (getTextChild(cond, "status").toInt())
		{
			ScenEvoTimeCondition *c = new ScenEvoTimeCondition(item_id, cond, has_next);
			QObject::connect(bt_global::btmain, SIGNAL(resettimer()), c, SLOT(setupTimer()));
			l.append(c);
		}

		QDomNode device = getElement(config_node, "scen/device");
		if (getTextChild(device, "objectID").toInt())
		{
			ScenEvoDeviceCondition *c = new ScenEvoDeviceCondition(item_id, device);
			l.append(c);
		}

		return l;
	}
#endif
}


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
		b = new scenEvo(0, item_node, loadConditions(item_node));
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

