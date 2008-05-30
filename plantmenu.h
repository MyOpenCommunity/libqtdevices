/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef HEADER_NAME_H
#define HEADER_NAME_H

#include "sottomenu.h"
#include "bannpuls.h"
#include "bannfullscreen.h"

#include <qdom.h>
#include <qptrvector.h>
#include <qsignalmapper.h>

class PlantMenu : public sottoMenu
{
Q_OBJECT
public:
	PlantMenu(QWidget *parent, char *name, QDomNode conf, QColor bg, QColor fg, QColor fg2);
private:
	bannPuls *addMenuItem(QDomNode n, const char *central_icon, QString descr, BannID type);
	/// Node in the Dom tree that is a `plant' tag
	QDomNode conf_root;
	/// Address of the thermal regulation device
	/// empty in case of 99z device
	QString ind_centrale;

	/// The second foreground color
	QColor second_fg;

	/*
	 * Used to call show(int) on SottoMenu based on which banner is clicked.
	 */
	QSignalMapper signal_mapper;

	/*
	 * Sub-sub menu used to show the full screen banners corresponding to the
	 * same small banners present in the plant menu.
	 */
	sottoMenu items_submenu;
};
#endif // HEADER_NAME_H
