#include "settings.h"
#include "bann_settings.h"
#include "alarmclock.h"
#include "global.h" // BTouch
#include "lansettings.h"
#include "multisounddiff.h" // contdiff
#include "xml_functions.h" // getChildren, getTextChild
#include "cleanscreen.h"
#include "impostatime.h"
#include "brightnesspage.h"

#include <QDebug>

#include <assert.h>

static const char *ICON_BRIGHTNESS = IMG_PATH "btlum.png";


Settings::Settings(QDomNode config_node)
{
	loadItems(config_node);
}

void Settings::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
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

			contdiff *cont = 0;
			if (type == AlarmClock::DI_SON)
				// Use old or multichannel sd
				if (BTouch->difSon)
					cont = new contdiff(BTouch->difSon, NULL);
				else if (BTouch->dm)
					cont = new contdiff(NULL, BTouch->dm);

			b = new bannAlarmClock(this, cont, hour, minute, img1, img2, img3, enabled, type, alarmset);
			break;
		}
		case SET_DATA_ORA:
			b = new bannOnDx(this, ICON_INFO, new impostaTime());
			break;
		case CONTRASTO:
			b = new bannContrast(this, getTextChild(item, "value"), img1);
			break;
		case CALIBRAZIONE:
		{
			b = new calibration(this, img1);
			connect(b, SIGNAL(startCalib()), this, SIGNAL(startCalib()));
			connect(b, SIGNAL(endCalib()), this, SIGNAL(endCalib()));
			break;
		}
		case PROTEZIONE:
			b = new impPassword(this, img1, img2, img3, getTextChild(item, "value"), getTextChild(item, "enabled").toInt());
			break;
		case VERSIONE:
			b = new bannVersion(this, img1, BTouch->datiGen);
			break;
		case BRIGHTNESS:
			b = new bannOnDx(this, ICON_BRIGHTNESS, new BrightnessPage());
			break;
		case CLEANSCREEN:
			b = new bannOnDx(this, ICON_INFO, new CleanScreen());
			break;
		case LANSETTINGS:
			b = new bannOnDx(this, ICON_INFO, new LanSettings(this));
			break;
		default:
			assert(!"Type of item not handled on settings page!");
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b);
	}
}
