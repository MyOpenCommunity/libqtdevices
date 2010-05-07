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


#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "banner.h"

class SourceDevice;
class StateButton;


class AudioSource : public BannerNew
{
Q_OBJECT
public:
	AudioSource(const QString &area, SourceDevice *dev, Page *details = NULL);

signals:
	void sourceStateChanged(bool active);

protected:
	void initBanner(const QString &left_on, const QString &left_off, const QString &center_left,
			const QString &center_right, const QString &right);

protected slots:
	void turnOn();
	void showDetails();

protected:
	QString area;
	SourceDevice *dev;
	Page *details;

	StateButton *left_button;
	BtButton *center_left_button, *center_right_button, *right_button;

private slots:
	void valueReceivedAudioSource(const DeviceValues &values_list);
};

#endif // AUDIOSOURCE_H
