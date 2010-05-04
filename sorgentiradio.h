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

#include "audiosource.h"

/// Forward Declarations
class RadioSourceDevice;
class RadioInfo;


/**
 * Banner for radio sources.
 */
class RadioSource : public AudioSource
{
Q_OBJECT
public:
	RadioSource(const QString &area, RadioSourceDevice *dev, Page *details);

private slots:
	void sourceStateChanged(bool active);
	void valueReceived(const DeviceValues &values_list);

private:
	RadioInfo *radio_info;
};

#endif
