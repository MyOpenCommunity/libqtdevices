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


class BannStopAndGo : public Bann2Buttons
{
Q_OBJECT
public:
	BannStopAndGo(StopAndGoDevice *dev, const QString &left, const QString &right, QWidget *parent = 0);

public slots:
	void statusChanged(const DeviceValues &values_list);

private:
	QMap<int, QString> status_icons;
};


class StopAndGoMenu : public BannerPage
{
Q_OBJECT
public:
	StopAndGoMenu(const QDomNode &conf_node);

	virtual void showPage();

private:
	Page *next_page;
};


class StopAndGoPage : public Page
{
Q_OBJECT
public:
	StopAndGoPage(const QString &title, StopAndGoDevice *device);

private slots:
	void turnOnOff(bool checked);

private:
	StopAndGoDevice *dev;
};


class StopAndGoPlusPage : public Page
{
Q_OBJECT
public:
	StopAndGoPlusPage(const QString &title, StopAndGoPlusDevice *device);

private:
	StopAndGoPlusDevice *dev;
};


class StopAndGoBTestPage : public Page
{
Q_OBJECT
public:
	StopAndGoBTestPage(const QString &title, StopAndGoBTestDevice *device);

private:
	StopAndGoBTestDevice *dev;
};

#endif
