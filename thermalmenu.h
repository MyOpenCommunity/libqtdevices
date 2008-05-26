/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief  A class to handle thermal regulation menu
 *
 *  This class represents the first level menu in thermal regulation.
 *  It's instantiated in xmlconfhandler.cpp and it reads by itself the
 *  configuration.
 *  It also creates all the necessary objects and submenus to handle
 *  all the items present in a thermal regulation page.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#ifndef BTOUCH_THERMALMENU_H
#define BTOUCH_THERMALMENU_H

#include "sottomenu.h"
#include "bannpuls.h"
#include "main.h"

class ThermalMenu : public sottoMenu
{
Q_OBJECT
public:
	/**
	 * 
	 */
	ThermalMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg);
protected:
	/**
	 * This function is called after a hide event is triggered
	 * by Qt.
	 */
	void hideEvent(QHideEvent *e);

private:
	QString getDeviceAddress(QDomNode);
	QDomNode findNamedNode(QDomNode, QString);
	bannPuls *addMenuItem(QDomElement, QString, QString);
	sottoMenu *createProbeMenu(QDomNode, bannPuls*);

	void addItems();
	void createPlantMenu();
	void addBanners();

	QDomNode conf_root;
	/// do NOT setAutoDelete(true), since banners are children of
	/// ThermalMenu and will be deleted by Qt
	QPtrList<banner> item_list;
};

#endif
