#include "settings.h"
#include "bann_settings.h"
#include "btmain.h" // bt_global::btmain
#include "lansettings.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "cleanscreen.h"
#include "changedatetime.h"
#include "brightnesspage.h"
#include "displaypage.h"
#include "version.h"
#include "bannercontent.h"
#include "main.h"
#include "skinmanager.h"

#include <QDebug>


Settings::Settings(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

banner *Settings::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	banner *b = 0;

	switch (id)
	{
	case SUONO:
		b = new impBeep(0, getTextChild(item_node, "value"),
				bt_global::skin->getImage("state_on"),
				bt_global::skin->getImage("state_off"));
		break;
	case SET_SVEGLIA_SINGLEPAGE:
	case SET_SVEGLIA:
	{
		int type = getTextChild(item_node, "type").toInt();
		int enabled = getTextChild(item_node, "enabled").toInt();
		int hour = getTextChild(item_node, "hour").toInt();
		int minute = getTextChild(item_node, "minute").toInt();
		QString days = getTextChild(item_node, "days");
		QList<bool> alarms;
		for (int i = 0; i < 7; ++i)
			alarms.append(false);
		for (int i = 0; i < days.length(); ++i)
		{
			int day = days.mid(i, 1).toInt() - 1;
			alarms[day] = true;
		}
		int alarmset = getTextChild(item_node, "alarmset").toInt();

		if (id == SET_SVEGLIA_SINGLEPAGE)
		{
			b = new bannAlarmClockIcon(hour, minute,
						    bt_global::skin->getImage("on"),
						    bt_global::skin->getImage("off"),
						    bt_global::skin->getImage("state_icon"),
						    bt_global::skin->getImage("edit"),
						    getTextChild(item_node, "descr"),
						    enabled, type, alarms);
		}
		else
		{
			b = new bannAlarmClock(0, hour, minute,
					       bt_global::skin->getImage("state_on"),
					       bt_global::skin->getImage("state_off"),
					       bt_global::skin->getImage("edit"),
					       enabled, type, alarmset);
		}

		break;
	}
	case SET_DATA_ORA:
		b = new bannOnDx(0, bt_global::skin->getImage("info"), new ChangeTime);
		break;
	case CONTRASTO:
		b = new bannContrast(0, getTextChild(item_node, "value"),
					 bt_global::skin->getImage("edit"));
		break;
	case DISPLAY:
		b = new bannOnDx(0, bt_global::skin->getImage("forward"), new DisplayPage(item_node));
		break;
	case PROTEZIONE:
		b = new impPassword(0,
					bt_global::skin->getImage("state_on"),
					bt_global::skin->getImage("state_off"),
					bt_global::skin->getImage("edit"),
					getTextChild(item_node, "value"), getTextChild(item_node, "enabled").toInt());
		break;
	case VERSIONE:
		b = new bannVersion(0, bt_global::skin->getImage("info"), bt_global::btmain->version);
		break;
	case LANSETTINGS:
		b = new bannOnDx(0, bt_global::skin->getImage("info"), new LanSettings(item_node));
		break;
	}

	if (b)
	{
		b->setText(getTextChild(item_node, "descr"));
		b->setId(id);
		b->Draw();
	}
	return b;
}

void Settings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
#ifdef BT_HARDWARE_X11
		if (id == CONTRASTO)
			continue;
#endif
		if (banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("Type of item %d not handled on settings page!", id);
	}
}

