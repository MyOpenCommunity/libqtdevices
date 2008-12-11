/*!
 * \file
 * <!--
 * Copyright 2008 MAC S.r.l. (http://www.mac-italia.com/)
 * All rights reserved.
 * -->
 *
 * \brief  Supervision main menu handling class
 *
 *  TODO: detailed description (optional) 
 *
 * \author Lucio Macellari
 */

#ifndef BTOUCH_SUPERVISION_MENU_H
#define BTOUCH_SUPERVISION_MENU_H


#include "sottomenu.h"
#include "bannpuls.h"
#include "main.h"
#include "stopngo.h"

#include <QWidget>
#include <QDomNode>
#include <QList>

/*!
  \class SupervisionMenu
  \brief Class defining a list of devices groups part of the supervision family.

  \author Lucio Macellari
  \date May 2008
*/
class SupervisionMenu : public sottoMenu
{
Q_OBJECT
public:
	SupervisionMenu(QDomNode config_node);
	~SupervisionMenu();

private:
	sottoMenu *stopngoSubmenu;
	QList<StopngoItem*> stopngoList;
	QList<StopngoPage*> stopngoPages;
	int classesCount;

	void Create2ButBanner(QDomElement, QString, QString);
	void CreateStopnGoMenu(QDomNode, bannPuls*);
	void loadItems(QDomNode config_node);
	void LinkBanner2Page(bannPuls*, StopngoItem*);

public slots:
	virtual void showPage();

signals:
	void quickOpen();
};

#endif
