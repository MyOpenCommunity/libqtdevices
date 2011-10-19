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
#include "multimedia_ts10.h"
#include "multimedia_filelist.h"
#include "messages.h"
#else
#include "settings_ts3.h"
#endif

#include "sounddiffusionpage.h"
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
#include "homepage.h"

#ifdef BUILD_EXAMPLES
#include "examples/tcpbanner/banner/tcpbannerpage.h"
#endif

#include <QObject>


Page *getSectionPageFromId(int id)
{
	Page *page = 0;
	
	//	MC20111006
	//  QDomNode page_node = getPageNode(id);
	//  Q_ASSERT_X(!page_node.isNull(), "getSectionPageFromId", qPrintable(QString("Page node null for id: %1").arg(id)));
	QDomNode page_node;
	
	if (bt_global::btmain->page_list.contains(id))
		return bt_global::btmain->page_list[id];
	else
	{
		page_node = getPageNode(id);
		Q_ASSERT_X(!page_node.isNull(), "getSectionPageFromId", qPrintable(QString("Page node null for id: %1").arg(id)));
	}

	switch (id)
	{
	case SPECIAL_PAGE:
		page = new SpecialPage(page_node);
		break;
	case HOME_PAGE:
		page = new HomePage(page_node);
		break;
	default:
		Q_ASSERT_X(false, "getSectionPageFromId", qPrintable(QString("Page %1 not handled!").arg(id)));
	}

	bt_global::btmain->page_list[id] = page;
	return page;
}


Page *getSectionPage(int page_id)
{
	//	MC20111006
	//	QDomNode page_node = getPageNodeFromPageId(page_id);
	//	int id = getTextChild(page_node, "id").toInt();
	//	Q_ASSERT_X(!page_node.isNull(), "getSectionPage", qPrintable(QString("Page node null for page id: %1").arg(page_id)));
	QDomNode page_node;
	int id=0;
	
	
	// A section page (with the same page_id) can be built only once.
	if (bt_global::btmain->page_list.contains(page_id))
		return bt_global::btmain->page_list[page_id];
	else
	{
		page_node = getPageNodeFromPageId(page_id);
		id = getTextChild(page_node, "id").toInt();
		Q_ASSERT_X(!page_node.isNull(), "getSectionPage", qPrintable(QString("Page node null for page id: %1").arg(page_id)));
	}

	
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
	case ENERGY_DATA:
		page = new EnergyData(page_node);
		break;
#ifdef LAYOUT_TS_10
	case MULTIMEDIA:
		page = new MultimediaSectionPage(page_node, MultimediaSectionPage::ITEMS_ALL,
			new MultimediaFileListFactory(TreeBrowser::DIRECTORY));
		break;
	case MESSAGES:
		page = new MessagesListPage(page_node);
		break;
#endif
	case SOUNDDIFFUSION:
	case SOUNDDIFFUSION_MULTI:
		page = new SoundDiffusionPage(page_node);
		break;
	case LOAD_MANAGEMENT:
		page = new LoadManagement(page_node);
		break;
#ifdef BUILD_EXAMPLES
	case TCP_BANNER_TEST:
		page = new TcpBannerPage;
		break;
#endif
	default:
		Q_ASSERT_X(false, "getSectionPage", qPrintable(QString("Page with page id %1 not handled!").arg(page_id)));
	}

	bt_global::btmain->page_list[page_id] = page;
	return page;
}

