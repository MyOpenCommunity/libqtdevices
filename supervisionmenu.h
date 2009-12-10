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

#include "page.h"
#include "stopngo.h"

#include <QDomNode>
#include <QList>

/*!
  \class SupervisionMenu
  \brief Class defining a list of devices groups part of the supervision family.

  \author Lucio Macellari
  \date May 2008
*/
class SupervisionMenu : public BannerPage
{
Q_OBJECT
public:
	SupervisionMenu(const QDomNode &config_node);
	~SupervisionMenu();

private:
	BannerPage *stopngoSubmenu;
	QList<StopngoItem*> stopngoList;
	QList<Page*> stopngoPages;
	int classesCount;

	void Create2ButBanner(QDomElement, QString, QString);
	void CreateStopnGoMenu(QDomNode, bannPuls*);
	void loadItems(const QDomNode &config_node);

public slots:
	virtual void showPage();

signals:
	void quickOpen();
};


class StopNGoMenu : public BannerPage
{
Q_OBJECT
public:
	StopNGoMenu(QList<StopngoItem*> stopngoList);
};

#endif
