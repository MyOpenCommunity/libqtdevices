#include "pagefactory.h"
#include "xml_functions.h" // getPageNode
#include "btmain.h" // bt_global::btmain
#include "sounddiffusion.h"
#include "multisounddiff.h"
#include "videoentryphone.h"
#ifdef LAYOUT_TOUCHX
#include "multimedia.h"
#endif
#include "antintrusion.h"
#include "automation.h"
#include "lighting.h"
#include "scenario.h"
#include "settings.h"
#include "loads.h"
#include "thermalmenu.h"
#include "supervisionmenu.h"
#include "specialpage.h"
#include "energy_data.h"
#include "openclient.h"
#include "iconpage.h"
#include "sectionpage.h"

#include <QObject>

Page *getPage(int page_id)
{
#ifdef CONFIG_BTOUCH
	QDomNode page_node = getPageNode(page_id);
	int id = page_id;
#else
	QDomNode page_node = getPageNodeFromPageId(page_id);
	int id = getTextChild(page_node, "id").toInt();
#endif
	if (page_node.isNull())
		return 0;

	// A section page can be built only once.
	if (bt_global::btmain->page_list.contains(page_id))
		return bt_global::btmain->page_list[page_id];

	Page *page = 0;
	switch (id)
	{
	case AUTOMAZIONE:
		page = new Automation(page_node);
		break;
	case ILLUMINAZIONE:
		page = new Lighting(page_node);
		break;
	case ANTIINTRUSIONE:
	{
		Antintrusion *p = new Antintrusion(page_node);
		QObject::connect(bt_global::btmain->client_comandi, SIGNAL(openAckRx()), p, SIGNAL(openAckRx()));
		QObject::connect(bt_global::btmain->client_comandi, SIGNAL(openNakRx()), p, SIGNAL(openNakRx()));
		page = p;
		break;
	}
	case CARICHI:
		page = new Loads(page_node);
		break;
	case TERMOREGOLAZIONE:
	case TERMOREG_MULTI_PLANT:
		page = new ThermalMenu(page_node);
		break;
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
		SectionPage *p = new SectionPage(page_node);
		p->addBackButton();
		page = p;
		break;
	}
	case SCENARI:
	case SCENARI_EVOLUTI:
		page = new Scenario(page_node);
		break;
	case IMPOSTAZIONI:
		page = new Settings(page_node);
		break;
	case VIDEOCITOFONIA:
		page = new VideoEntryPhone(page_node);
		break;
	case SUPERVISIONE:
	{
		SupervisionMenu *p = new SupervisionMenu(page_node);
		p->forceDraw();
		QObject::connect(p, SIGNAL(richStato(QString)), bt_global::btmain->client_richieste, SLOT(richStato(QString)));
		page = p;
		break;
	}
	case SPECIAL:
		page = new SpecialPage(page_node);
		break;
	case ENERGY_DATA:
		page = new EnergyData(page_node);
		break;
#ifdef LAYOUT_TOUCHX
	case MULTIMEDIA:
		page = new MultimediaSectionPage(page_node);
		break;
#endif
	default:
		qFatal("Page %d not found on xml config file!", id);
	}

	bt_global::btmain->page_list[id] = page;
	return page;
}

