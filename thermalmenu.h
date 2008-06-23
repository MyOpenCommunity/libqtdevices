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
	ThermalMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg, QColor fg2);

private:
	QString getDeviceAddress(QDomNode);
	bannPuls *addMenuItem(QDomElement, QString, QString);
	/**
	 * Create a sottoMenu to show external and not controlled probes
	 *
	 * \param config    The node in the Dom tree that holds the `probe' section
	 * \param bann      A pointer to the banner that gives access to the
	 * sottoMenu
	 * \param external  True if the probe is external, false otherwise
	 */
	void createProbeMenu(QDomNode config, bannPuls *bann, bool external);

	void addItems();
	void createPlantMenu(QDomNode config, bannPuls *bann);
	void addBanners();

	QDomNode conf_root;
	/// do NOT setAutoDelete(true), since banners are children of
	/// ThermalMenu and will be deleted by Qt
	QColor second_fg;
};

#endif
