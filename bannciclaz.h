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


#ifndef BANNCICLAZ_H
#define BANNCICLAZ_H

#include "banner.h"

#include <QWidget>

#define BANCICL_BUT_DIM_X 60
#define BANCICL_BUT_DIM_Y 60
#define BANCICL_H_SCRITTA 20

/*!
 * \class bann4tasLab
 * \brief This is a class that describes a banner with 4 buttons and a text over the two internal buttons.
 *
 * The two external buttons are big while the internal ones leave the space to host the descripting text over them.
 * \date lug 2005
 */
class bannCiclaz : public BannerOld
{
Q_OBJECT
public:
     bannCiclaz(QWidget *parent, int nbuts=4);
};

#endif //BANNCICLAZ_H
