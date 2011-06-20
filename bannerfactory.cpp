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


#include "bannerfactory.h"
#include "banner.h"
#include "lighting.h"
#include "automation.h"
#include "scenario.h"
#ifdef LAYOUT_TS_10
#include "settings_ts10.h"
#else
#include "settings_ts3.h"
#endif


banner *getBanner(const QDomNode &item_node)
{
	typedef banner* (*getBannerFunc)(const QDomNode &);
	QList<getBannerFunc> sections;
	sections.append(&Automation::getBanner);
	sections.append(&Lighting::getBanner);
	sections.append(&Scenario::getBanner);
#ifdef LAYOUT_TS_10
	sections.append(&IconSettings::getBanner);
#else
	sections.append(&Settings::getBanner);
#endif

	for (int i = 0; i < sections.size(); ++i)
		if (banner *b = sections[i](item_node))
			return b;

	return 0;
}
