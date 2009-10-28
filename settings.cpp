#include "settings.h"
#include "bann_settings.h"
#include "btmain.h" // bt_global::btmain
#include "lansettings.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "cleanscreen.h"
#include "impostatime.h"
#include "brightnesspage.h"
#include "displaypage.h"
#include "version.h"
#include "content_widget.h" // content_widget
#include "main.h"
#include "skinmanager.h"

#include <QDebug>


Settings::Settings(const QDomNode &config_node)
{
	buildPage();
	loadItems(config_node);
}

void Settings::inizializza()
{
	content_widget->initBanners();
}

void Settings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item, "cid").toInt());

		int id = getTextChild(item, "id").toInt();
		banner *b;
		switch (id)
		{
		case SUONO:
			b = new impBeep(this, getTextChild(item, "value"),
					bt_global::skin->getImage("state_on"),
					bt_global::skin->getImage("state_off"));
			break;
		case SET_SVEGLIA:
		{
			int type = getTextChild(item, "type").toInt();
			int enabled = getTextChild(item, "enabled").toInt();
			int alarmset = getTextChild(item, "alarmset").toInt();
			int hour = getTextChild(item, "hour").toInt();
			int minute = getTextChild(item, "minute").toInt();

			b = new bannAlarmClock(this, hour, minute,
					       bt_global::skin->getImage("state_on"),
					       bt_global::skin->getImage("state_off"),
					       bt_global::skin->getImage("edit"),
					       enabled, type, alarmset);
			break;
		}
		case SET_DATA_ORA:
			b = new bannOnDx(this, bt_global::skin->getImage("info"), new ChangeTime());
			break;
#if !defined(BT_HARDWARE_X11)
		case CONTRASTO:
			b = new bannContrast(this, getTextChild(item, "value"),
					     bt_global::skin->getImage("edit"));
			break;
		case DISPLAY:
			b = new bannOnDx(this, bt_global::skin->getImage("forward"), new DisplayPage(item));
			break;
#else
		case CONTRASTO:
		case DISPLAY:
			continue;
#endif
		case PROTEZIONE:
			b = new impPassword(this,
					    bt_global::skin->getImage("state_on"),
					    bt_global::skin->getImage("state_off"),
					    bt_global::skin->getImage("edit"),
					    getTextChild(item, "value"), getTextChild(item, "enabled").toInt());
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
			break;
		case VERSIONE:
			b = new bannVersion(this, bt_global::skin->getImage("info"), bt_global::btmain->version);
			break;
		case LANSETTINGS:
			b = new bannOnDx(this, bt_global::skin->getImage("info"), new LanSettings(item));
			break;
		default:
			qFatal("Type of item not handled on settings page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		b->Draw();
		content_widget->appendBanner(b);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}
