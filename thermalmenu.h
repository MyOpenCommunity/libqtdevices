/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief  A class to handle thermal regulation menu
 *
 *  TODO: detailed description (optional) 
 *
 * \author Luca Ottaviano
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
	ThermalMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg);

private:
	QString getDeviceAddress(QDomNode);
	QDomNode findNamedNode(QDomNode, QString);
	void create2ButBanner(QDomElement, QString, QString);
	void createProbeMenu(QDomNode, bannPuls*);

	void addItems();
	void createPlantMenu();
	void addBanners();

	QDomNode subtreeRoot;
};

#endif
