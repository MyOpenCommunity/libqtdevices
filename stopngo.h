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


#ifndef STOPNGO_H
#define STOPNGO_H

#include "bannerpage.h"
#include "bann2_buttons.h"


class StopAndGoDevice;
class StopAndGoPlusDevice;
class StopAndGoBTestDevice;
class QDomNode;


/**
 * BannStopAndGo
 *
 * Banner connected to a stop and go device.
 * It is composed by a central image, two optionals buttons and an optional
 * description.
 *
 * The central image automatically changes to reflect the device status.
 */
class BannStopAndGo : public Bann2Buttons
{
Q_OBJECT
public:
	BannStopAndGo(StopAndGoDevice *dev, const QString &left, const QString &right, const QString &descr = QString(), QWidget *parent = 0);

public slots:
	void valueReceived(const DeviceValues &values_list);

private:
	QMap<int, QString> status_icons;
};


/**
 * StopAndGoMenu
 *
 * This page list all the configured stop and go pages.
 * If there is only one stop and go page, this page isn't displayed and will
 * forward directly to the stop and go page.
 */
class StopAndGoMenu : public BannerPage
{
Q_OBJECT
public:
	StopAndGoMenu(const QDomNode &config_node);

	virtual void showPage();

private:
	void loadItems(const QDomNode &config_node);

	Page *next_page;
};


/**
 * StopAndGoPage
 *
 * This page contains a banner which reflects the device status and a button
 * used to enable or disable the autoreset.
 */
class StopAndGoPage : public Page
{
Q_OBJECT
public:
	StopAndGoPage(const QString &title, StopAndGoDevice *device);

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();

private:
	StopAndGoDevice *dev;
	StateButton *autoreset_button;
};


/**
 * StopAndGoPlusPage
 *
 * This page is like the StopAndGoPage, but in addiction, permits to open
 * and close, and track fails.
 */
class StopAndGoPlusPage : public Page
{
Q_OBJECT
public:
	StopAndGoPlusPage(const QString &title, StopAndGoPlusDevice *device);

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();
	void switchTracking();

private:
	StopAndGoPlusDevice *dev;
	StateButton *autoreset_button;
	StateButton *tracking_button;
};


/**
 * StopAndGoBTestPage
 *
 * This page is like the StopAndGoPage, but in addiction, permits to switch on
 * and off the self tests, and to set the interval in days to perform
 * the tests.
 */
class StopAndGoBTestPage : public Page
{
Q_OBJECT
public:
	StopAndGoBTestPage(const QString &title, StopAndGoBTestDevice *device);

private slots:
	void valueReceived(const DeviceValues &values_list);
	void switchAutoReset();
	void switchAutoTest();

private:
	StopAndGoBTestDevice *dev;
	StateButton *autoreset_button;
	StateButton *autotest_button;
	BannLCDRange *autotest_banner;
};

#endif
