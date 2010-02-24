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


#include "bannciclaz.h"

#define BANCICL_BUT_DIM_X 60
#define BANCICL_BUT_DIM_Y 60
#define BANCICL_H_SCRITTA 20


bannCiclaz::bannCiclaz(QWidget *parent,int nbut) : BannerOld(parent)
{
	addItem(BUT1, 0, (banner_height - BANCICL_BUT_DIM_Y)/2, BANCICL_BUT_DIM_X, BANCICL_BUT_DIM_Y);
	addItem(BUT2, banner_width-BANCICL_BUT_DIM_X, (banner_height-BANCICL_BUT_DIM_Y)/2, BANCICL_BUT_DIM_X, BANCICL_BUT_DIM_Y);

	if (nbut == 4) 
	{
		addItem(BUT3, BANCICL_BUT_DIM_X, BANCICL_H_SCRITTA, (banner_width-2*BANCICL_BUT_DIM_X)/2,
			banner_height- BANCICL_H_SCRITTA);
		addItem(BUT4, banner_width / 2, BANCICL_H_SCRITTA, (banner_width-  2*BANCICL_BUT_DIM_X)/2,
			 banner_height - BANCICL_H_SCRITTA);
	}
	else if (nbut == 3)
	{
		addItem(BUT3, banner_width/2 - BANCICL_BUT_DIM_X/2, BANCICL_H_SCRITTA, BANCICL_BUT_DIM_X,
			banner_height - BANCICL_H_SCRITTA);
	}

	addItem(TEXT, BANCICL_BUT_DIM_X, 0, banner_width - 2*BANCICL_BUT_DIM_X, BANCICL_H_SCRITTA);
}

