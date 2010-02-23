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


#ifndef BTOUCH_SUPERVISION_MENU_H
#define BTOUCH_SUPERVISION_MENU_H


#include "sottomenu.h"
#include "bann1_button.h"
#include "main.h"
#include "stopngo.h"

#include <QWidget>
#include <QDomNode>
#include <QList>

/*!
  \class SupervisionMenu
  \brief Class defining a list of devices groups part of the supervision family.

  \date May 2008
*/
class SupervisionMenu : public sottoMenu
{
Q_OBJECT
public:
	SupervisionMenu(const QDomNode &config_node);
	~SupervisionMenu();

private:
	sottoMenu *stopngoSubmenu;
	QList<StopngoItem*> stopngoList;
	QList<StopngoPage*> stopngoPages;
	int classesCount;

	void Create2ButBanner(QDomElement, QString, QString);
	void CreateStopnGoMenu(QDomNode, bannPuls*);
	void loadItems(const QDomNode &config_node);
	void LinkBanner2Page(bannPuls*, StopngoItem*);

public slots:
	virtual void showPage();

signals:
	void quickOpen();
};

#endif
