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


#ifndef AMPLIFICATORI_H
#define AMPLIFICATORI_H

#include "bannregolaz.h"

#include <QWidget>
#include <QString>
#include <QList>

class device_status;
class device;
class AmplifierDevice;


class Amplifier : public BannLevel
{
Q_OBJECT
public:
	Amplifier(const QString &descr, const QString &where);

private:
	void setIcons();

private slots:
	void volumeUp();
	void volumeDown();
	void turnOn();
	void turnOff();

	void valueReceived(const DeviceValues &values_list);

private:
	QString center_left_active, center_left_inactive, center_right_active, center_right_inactive;
	int volume_value;
	bool active;
	AmplifierDevice *dev;
};


class AmplifierGroup : public BannLevel
{
Q_OBJECT
public:
	AmplifierGroup(QStringList addresses, const QString &descr);

private slots:
	void volumeUp();
	void volumeDown();
	void turnOn();
	void turnOff();

private:
	QString center_left_active, center_left_inactive, center_right_active, center_right_inactive;
	QList<AmplifierDevice*> devices;
};

#endif
