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
#include "main.h"

class ThermalMenu : public sottoMenu
{
Q_OBJECT
public:
	ThermalMenu(QWidget *parent, const char *name, QDomNode n);

private:
	QString getDeviceAddress(QDomNode root);
	QDomNode findNamedNode(QDomNode root, QString name);

	void addItems();
	void createPlantMenu();
	void addBanners();

	QDomNode root;



};

#endif
