#include "settings.h"
#include "setitems.h"
#include "sveglia.h"
#include "main.h"
#include "btmain.h" // BTouch
#include "lansettings.h"
#include "diffmulti.h" // contdiff
#include "xml_functions.h" // getChildren, getTextChild
#include "cleanscreen.h"
#include "impostatime.h"

#include <QDebug>


Settings::Settings(QWidget *parent, QDomNode config_node) : sottoMenu(parent)
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
		QString img4 = IMG_PATH + getTextChild(item, "cimg4");
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
			QString hour = getTextChild(item, "hour");
			QString minute = getTextChild(item, "minute");

			contdiff *cont = 0;
			if (type == sveglia::DI_SON)
				// Use old or multichannel sd
				if (BTouch->difSon)
					cont = new contdiff(BTouch->difSon, NULL);
				else if (BTouch->dm)
					cont = new contdiff(NULL, BTouch->dm);

			b = new impostaSveglia(this, cont, hour, minute, img1, img2, img3, enabled, type, alarmset);
			break;
			}
		case SET_DATA_ORA:
			b = new bannOnDx(this, ICON_INFO, new impostaTime());
			break;
		case CONTRASTO:
			b = new impContr(this, getTextChild(item, "value"), img1);
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
			b = new machVers(this, BTouch->datiGen, img1);
			break;
		case BRIGHTNESS:
			b = new BannBrightness(this);
			break;
		case CLEANSCREEN:
			b = new bannOnDx(this, ICON_INFO, new CleanScreen());
			break;
		case LANSETTINGS:
			b = new bannOnDx(this, ICON_INFO, new LanSettings(this));
			break;
		}
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b);
	}
}
