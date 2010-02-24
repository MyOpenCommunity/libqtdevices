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


#ifndef ALARMSOUNDDIFF_DEVICE_H
#define ALARMSOUNDDIFF_DEVICE_H

#include "device.h"


class AlarmSoundDiffDevice : public device
{
friend class TestAlarmSoundDiffDevice;
Q_OBJECT
public:

	enum Type
	{
		DIM_AMPLIFIER = 1,
		DIM_STATUS = 2,
		DIM_VOLUME = 3,
		DIM_SOURCE = 4,
		DIM_RADIO_STATION = 5
	};

	AlarmSoundDiffDevice();

	void startAlarm(int source, int radio_station, int *alarmVolumes);
	void stopAlarm(int source, int *alarmVolumes);
	void setVolume(int amplifier, int volume);

	void setReceiveFrames(bool receive);

protected:
	virtual void manageFrame(OpenMsg &msg);

private:
	void setRadioStation(int source, int radio_station);
	// TODO check function name
	void activateEnvironment(int environment, int source);
	void activateSource(int source);
	void amplifierOn(int amplifier);
	void amplifierOff(int amplifier);

private:
	bool receive_frames;
};

#endif // ALARMSOUNDDIFF_DEVICE_H
