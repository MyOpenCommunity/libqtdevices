/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "pagefactory.h"
#include "main.h"
#include "xml_functions.h" // getPageNode
#include "btmain.h" // bt_global::btmain
#include "videodoorentry.h"
#ifdef LAYOUT_TS_10
#include "multimedia.h"
#include "multimedia_filelist.h"
#include "messages.h"
#include "sounddiffusionpage.h"
#else
#include "settings.h"
#endif
#include "antintrusion.h"
#include "automation.h"
#include "lighting.h"
#include "scenario.h"
#include "loads.h"
#include "thermalmenu.h"
#include "supervisionmenu.h"
#include "specialpage.h"
#include "energy_data.h"
#include "iconpage.h"
#include "energy_management.h"
#include "load_management.h"


#include <QObject>

Page *getPage(int page_id)
{
#ifdef CONFIG_TS_3_5
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
	case AUTOMATION:
		page = new Automation(page_node);
		break;
	case LIGHTING:
		page = new Lighting(page_node);
		break;
	case ANTIINTRUSION:
		page = new Antintrusion(page_node);
		break;
	case LOADS:
		page = new Loads(page_node);
		break;
	case THERMALREGULATION:
	case THERMALREGULATION_MULTI:
		page = new ThermalMenu(page_node);
		break;
	case ENERGY_MANAGEMENT:
	{
		EnergyManagement *p = new EnergyManagement(page_node);
		page = p;
		break;
	}
	case SCENARIOS:
	case EVOLVED_SCENARIOS:
		page = new Scenario(page_node);
		break;
#ifdef LAYOUT_TS_3_5
	case SETTINGS:
		page = new Settings(page_node);
		break;
#endif
	case VIDEODOORENTRY:
		page = new VideoDoorEntry(page_node);
		break;
	case SUPERVISION:
		page = new SupervisionMenu(page_node);
		break;
	case SPECIAL:
		page = new SpecialPage(page_node);
		break;
	case ENERGY_DATA:
		page = new EnergyData(page_node);
		break;
#ifdef LAYOUT_TS_10
	case MULTIMEDIA:
		page = new MultimediaSectionPage(page_node, MultimediaSectionPage::ITEMS_ALL, new MultimediaFileListPage);
		break;
	case MESSAGES:
		page = new MessagesListPage(page_node);
		break;
	case SOUNDDIFFUSION:
	case SOUNDDIFFUSION_MULTI:
		page = new SoundDiffusionPage(page_node);
		break;
#endif

	case LOAD_MANAGEMENT:
		page = new LoadManagement(page_node);
		break;
	default:
		qFatal("Page %d not handled!", id);
	}

	bt_global::btmain->page_list[page_id] = page;
	return page;
}

