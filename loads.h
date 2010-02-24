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


#ifndef LOADS_H
#define LOADS_H

#include "bann1_button.h" // class bannOnSx
#include "page.h"

class QDomNode;

/*!
 * \class bannLoads
 * \brief This class is made to force an appliance.
 *
 */
class bannLoads : public bannOnSx
{
Q_OBJECT
public:
	bannLoads(Page *parent, QString indirizzo, QString IconaSx);
private slots:
	void Attiva();
};


class Loads : public BannerPage
{
Q_OBJECT
public:
	Loads(const QDomNode &config_node);
private:
	void loadItems(const QDomNode &config_node);
};

#endif
