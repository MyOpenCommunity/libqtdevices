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


#ifndef BTOUCH_THERMALMENU_H
#define BTOUCH_THERMALMENU_H

#include "bannerpage.h"
#include "bann1_button.h"
#include "bttime.h"

#include <QDomNode>
#include <QWidget>
#include <QString>


class ThermalMenu : public BannerPage
{
Q_OBJECT
public:
	/**
	 * 
	 */
	ThermalMenu(const QDomNode &config_node);

	virtual int sectionId() const;

public slots:
	/**
	 * Show ThermalMenu if there are two or more banners, show the only submenu below us
	 * otherwise.
	 */
	virtual void showPage();

private:
	BannSinglePuls *addMenuItem(QDomElement, QString);
	/**
	 * Create a banner list to show external and not controlled probes
	 *
	 * \param config    The node in the Dom tree that holds the `probe' section
	 * \param bann      A pointer to the banner that gives access to the
	 * subpage
	 * \param external  True if the probe is external, false otherwise
	 */
	void createProbeMenu(QDomNode config, BannSinglePuls *bann, bool external);

	void createPlantMenu(QDomNode config, BannSinglePuls *bann);
	void loadBanners(const QDomNode &config_node);

	/// do NOT setAutoDelete(true), since banners are children of
	/// ThermalMenu and will be deleted by Qt

	unsigned bann_number;
	/// A reference to the only submenu below us
	Page *single_page;
};


class ProbesPage : public BannerPage
{
Q_OBJECT
public:
	ProbesPage(const QDomNode &config_node, bool are_probes_external);

	virtual int sectionId() const;

private:
	void loadItems(const QDomNode &config_node, bool are_probes_external);
};

#endif
