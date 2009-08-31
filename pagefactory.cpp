#include "pagefactory.h"
#include "xml_functions.h" // getPageNode
#include "btmain.h" // bt_global::btmain
#include "sounddiffusion.h"
#include "multisounddiff.h"
#include "videoentryphone.h"
#include "antintrusion.h"
#include "automation.h"
#include "lighting.h"
#include "scenario.h"
#include "settings.h"
#include "loads.h"
#include "thermalmenu.h"
#include "feedmanager.h"
#include "supervisionmenu.h"
#include "specialpage.h"
#include "energy_data.h"
#include "openclient.h"
#include "pagecontainer.h"

#include <QObject>

Page *getPage(int id)
{
	QDomNode page_node = getPageNode(id);
	if (page_node.isNull())
		return 0;

	// A section page can be built only once.
	if (bt_global::btmain->page_list.contains(id))
		return bt_global::btmain->page_list[id];

	Page *page = 0;
	switch (id)
	{
	case AUTOMAZIONE:
	{
		Automation *p = new Automation(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case ILLUMINAZIONE:
	{
		Lighting *p = new Lighting(page_node);
		p->forceDraw();
		QObject::connect(p, SIGNAL(richStato(QString)), bt_global::btmain->client_richieste, SLOT(richStato(QString)));
		page = p;
		break;
	}
	case ANTIINTRUSIONE:
	{
		Antintrusion *p = new Antintrusion(page_node);
		p->draw();
		QObject::connect(bt_global::btmain->client_comandi, SIGNAL(openAckRx()), p, SIGNAL(openAckRx()));
		QObject::connect(bt_global::btmain->client_comandi, SIGNAL(openNakRx()), p, SIGNAL(openNakRx()));
		page = p;
		break;
	}
	case CARICHI:
	{
		Loads *p = new Loads(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case TERMOREGOLAZIONE:
	case TERMOREG_MULTI_PLANT:
	{
		ThermalMenu *p = new ThermalMenu(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case DIFSON:
	{
		SoundDiffusion *p = new SoundDiffusion(page_node);
		p->draw();
		page = p;

		if (!bt_global::btmain->difSon)
		{
			SoundDiffusionAlarm *sd = new SoundDiffusionAlarm(p->getAudioSources(), page_node);
			sd->forceDraw();
			bt_global::btmain->difSon = sd;
		}
		break;
	}
	case DIFSON_MULTI:
	{
		MultiSoundDiff *p = new MultiSoundDiff(page_node);
		p->forceDraw();
		page = p;

		if (!bt_global::btmain->dm)
		{
			MultiSoundDiffAlarm *tmp = new MultiSoundDiffAlarm(page_node);
			tmp->forceDraw();
			bt_global::btmain->dm = tmp;
		}
		break;
	}
	case ENERGY_MANAGEMENT:
	{
		PageContainer *p = new PageContainer(page_node);
		p->addBackButton();
		page = p;
		break;
	}
	case SCENARI:
	case SCENARI_EVOLUTI:
	{
		Scenario *p = new Scenario(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case IMPOSTAZIONI:
	{
		Settings *p = new Settings(page_node);
		p->forceDraw();
		QObject::connect(p, SIGNAL(startCalib()), bt_global::btmain, SLOT(startCalib()));
		QObject::connect(p, SIGNAL(endCalib()), bt_global::btmain, SLOT(endCalib()));
		page = p;
		break;
	}
	case VIDEOCITOFONIA:
	{
		VideoEntryPhone *p = new VideoEntryPhone(page_node);
		p->forceDraw();
		page = p;
		break;
	}
	case SUPERVISIONE:
	{
		SupervisionMenu *p = new SupervisionMenu(page_node);
		p->forceDraw();
		QObject::connect(bt_global::btmain->client_monitor, SIGNAL(frameIn(char *)), p, SIGNAL(gestFrame(char *)));
		QObject::connect(p, SIGNAL(richStato(QString)), bt_global::btmain->client_richieste, SLOT(richStato(QString)));
		page = p;
		break;
	}
	case SPECIAL:
	{
		SpecialPage *p = new SpecialPage(page_node);
		page = p;
		break;
	}
	case ENERGY_DATA:
	{
		EnergyData *p = new EnergyData(page_node);
		page = p;
		break;
	}
	case FEED_READER:
	{
		FeedManager *p = new FeedManager;
		page = p;
		break;
	}
	default:
		qFatal("Page %d not found on xml config file!", id);
	}

	bt_global::btmain->page_list[id] = page;
	return page;
}

