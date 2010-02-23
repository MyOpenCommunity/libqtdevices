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


#ifndef BT_HOMEPAGE
#define BT_HOMEPAGE

#include "pagecontainer.h"

#include <QString>

class timeScript;
class TemperatureViewer;

class QDomNode;


/*!
  \class homePage
  \brief This is the class used to implement the main page from which access to all the subtree (sottoMenu) of the various bticino functions. This class is also used for the special page.

  It can be composed of a date field, a time field, up to three temperatures, a number of BtButton and a special command. 
  \author Davide
  \date lug 2005
*/

class homePage : public PageContainer
{
Q_OBJECT
public:
	homePage(const QDomNode &config_node);
	void inizializza();

private slots:
	void gestFrame(char*);

private:
	TemperatureViewer *temp_viewer;
	void loadItems(const QDomNode &config_node);
};


#endif // BT_HOMEPAGE
