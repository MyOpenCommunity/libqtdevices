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


#ifndef SORGENTI_RADIO_H
#define SORGENTI_RADIO_H

#include "bannciclaz.h"
#include "audiosource.h"
#include "device_status.h"

#include <QList>
#include <QString>
#include <QStringList>

/// Forward Declarations
class radio;
class device;
class RadioSourceDevice;


/**
 * Banner for radio sources.
 */
class RadioSource : public AudioSource
{
Q_OBJECT
public:
	RadioSource(const QString &area, RadioSourceDevice *dev, Page *details);

protected:
	void initBanner(const QString &left, const QString &center_left, const QString &center, const QString &center_right,
		const QString &right);

private slots:
	void sourceStateChanged(bool active);
	void valueReceived(const DeviceValues &values_list);

private:
	BtButton *left_button, *center_left_button, *center_right_button, *right_button;
	QLabel *background, *radio_station, *radio_frequency, *radio_channel;
};

#endif
