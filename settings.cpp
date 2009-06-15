#include "settings.h"
#include "bann_settings.h"
#include "alarmclock.h"
#include "btmain.h" // bt_global::btmain
#include "lansettings.h"
#include "multisounddiff.h" // contdiff
#include "xml_functions.h" // getChildren, getTextChild
#include "cleanscreen.h"
#include "impostatime.h"
#include "brightnesspage.h"
#include "displaypage.h"
#include "version.h"

#include <QDebug>

#include <assert.h>


Settings::Settings(const QDomNode &config_node)
{
	loadItems(config_node);
}

void Settings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		QString img2 = IMG_PATH + getTextChild(item, "cimg2");
		QString img3 = IMG_PATH + getTextChild(item, "cimg3");
		banner *b;
		switch (id)
		{
		case SUONO:
			b = new impBeep(this, getTextChild(item, "value"), img1, img2);
			break;
		case SET_SVEGLIA:
		{
			int type = getTextChild(item, "type").toInt();
			int enabled = getTextChild(item, "enabled").toInt();
			int alarmset = getTextChild(item, "alarmset").toInt();
			int hour = getTextChild(item, "hour").toInt();
			int minute = getTextChild(item, "minute").toInt();

			b = new bannAlarmClock(this, hour, minute, img1, img2, img3, enabled, type, alarmset);
			break;
		}
		case SET_DATA_ORA:
			b = new bannOnDx(this, ICON_INFO, new impostaTime());
			break;
		case CONTRASTO:
			b = new bannContrast(this, getTextChild(item, "value"), img1);
			break;
		case DISPLAY:
			b = new bannOnDx(this, ICON_FRECCIA_DX, new DisplayPage(item));
			break;
		case PROTEZIONE:
			b = new impPassword(this, img1, img2, img3, getTextChild(item, "value"), getTextChild(item, "enabled").toInt());
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
			break;
		case VERSIONE:
			b = new bannVersion(this, img1, bt_global::btmain->version);
			break;
		case LANSETTINGS:
			b = new bannOnDx(this, ICON_INFO, new LanSettings(item));
			break;
		default:
			assert(!"Type of item not handled on settings page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}
