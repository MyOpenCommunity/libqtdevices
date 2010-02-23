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


#include "bannonoffcombo.h"
#include "main.h"

#include <QLabel>

#define BUTONOFFCOMBO_ICON_DIM_Y 60
#define BANONOFFCOMBO_BUT_DIM 60

#define BUTONOFFCOMBO_ICON_DIM_MAX_X 120
#define BUTONOFFCOMBO_ICON_DIM_MIN_X 80


BannOnOffCombo::BannOnOffCombo(QWidget *parent) : banner(parent)
{
	addItem(BUT1, MAX_WIDTH-BANONOFFCOMBO_BUT_DIM, 0, BANONOFFCOMBO_BUT_DIM, BANONOFFCOMBO_BUT_DIM);
	addItem(BUT2, 0, 0, BANONOFFCOMBO_BUT_DIM, BANONOFFCOMBO_BUT_DIM);
	addItem(TEXT, 0, BANONOFFCOMBO_BUT_DIM, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE-BANONOFFCOMBO_BUT_DIM);
	addItem(ICON, BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MAX_X, BUTONOFFCOMBO_ICON_DIM_Y);
	addItem(TEXT2, BANONOFFCOMBO_BUT_DIM, 0, MAX_WIDTH-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
	nascondi(TEXT2);
}

void BannOnOffCombo::changeStatus(ComboStatus st)
{
	banner::SetIcons(2, status_icon[st]);

	switch (st)
	{
	case CENTER:
		BannerIcon->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MAX_X,BUTONOFFCOMBO_ICON_DIM_Y);
		nascondi(TEXT2);
		break;
	case SX:
		BannerIcon->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, BUTONOFFCOMBO_ICON_DIM_MIN_X,BUTONOFFCOMBO_ICON_DIM_Y);
		SecondaryText->setGeometry(BANONOFFCOMBO_BUT_DIM+BUTONOFFCOMBO_ICON_DIM_MIN_X, 0, MAX_WIDTH-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
		mostra(TEXT2);
		break;
	case DX:
		BannerIcon->setGeometry(MAX_WIDTH-BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, 0, BUTONOFFCOMBO_ICON_DIM_MIN_X,BUTONOFFCOMBO_ICON_DIM_Y);
		SecondaryText->setGeometry(BANONOFFCOMBO_BUT_DIM, 0, MAX_WIDTH-2*BANONOFFCOMBO_BUT_DIM-BUTONOFFCOMBO_ICON_DIM_MIN_X, BUTONOFFCOMBO_ICON_DIM_Y);
		mostra(TEXT2);
		break;
	default:
		qWarning("STATUS UNKNOWN on BannOnOffCombo::changeStatus");
	}
}

void BannOnOffCombo::SetIcons(QString sxIcon, QString dxIcon, QString centerIcon, QString centerSxIcon, QString centerDxIcon)
{
	status_icon[CENTER] = centerIcon;
	status_icon[SX] = centerSxIcon;
	status_icon[DX] = centerDxIcon;
	banner::SetIcons(sxIcon, dxIcon, QString(), centerIcon);
}
