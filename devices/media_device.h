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

#ifndef MEDIA_DEVICE_H
#define MEDIA_DEVICE_H

#include "device.h"

#include <QSet>
#include <QString>


/**
 * This class implements a multimedia source device, that can be used to send
 * command to the actual device or notified status changed.
 */
class SourceDevice : public device
{
friend class TestSourceDevice;
Q_OBJECT
public:
	enum
	{
		DIM_STATUS = 12,
		DIM_TRACK = 6,
		REQ_NEXT_TRACK = 9,
		REQ_PREV_TRACK = 10,
		DIM_AREAS_UPDATED = -2,
	};

	SourceDevice(QString source, int openserver_id = 0);

public slots:
	virtual void nextTrack();
	virtual void prevTrack();
	void turnOn(QString area) const;
	void requestTrack() const;

public:
	bool isActive(QString area) const;

protected:
	int mmtype;
	QString source_id;
	QSet<QString> active_areas;

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private slots:
	void requestActiveAreas() const;
};


/**
 * This class implements a radio source device, that can be used to communicate
 * with the actual radio source.
 */
class RadioSourceDevice : public SourceDevice
{
Q_OBJECT
public:
	enum
	{
		DIM_FREQUENCY = 5,
		DIM_RDS = 10,
	};

	RadioSourceDevice(QString source_id, int openserver_id = 0);

public slots:
	void frequenceUp(QString value = QString()) const;
	void frequenceDown(QString value = QString()) const;
	void saveStation(QString station) const;
	void setStation(QString station) const;

	void requestFrequency() const;
	void requestRDS() const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};


/**
 * This class implements a virtual multimedia source device, that can manage
 * command received like an actual source device.
 */
class VirtualSourceDevice : public SourceDevice
{
Q_OBJECT
public:
	enum
	{
		REQ_SOURCE_ON,
		REQ_SOURCE_OFF,
	};

	VirtualSourceDevice(QString address, int openserver_id = 0);

	virtual void nextTrack();
	virtual void prevTrack();

	static QString createMediaInitFrame(bool is_multichannel, const QString &source_addr, const QString &ampli_addr);

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};


/**
 * This class represents an amplifier device, that can be used to reproduce audio
 * from various kind of sources inside an area.
 */
class AmplifierDevice : public device
{
Q_OBJECT
public:
	enum
	{
		DIM_STATUS = 12,
		DIM_VOLUME = 1,
	};

	// passing "0" as the where creates a device commanding all amplifiers
	AmplifierDevice(QString where, int openserver_id = 0);
	AmplifierDevice(QString area, QString point, int openserver_id = 0);

	virtual void init();

	void requestStatus() const;
	void requestVolume() const;
	virtual void turnOn();
	virtual void turnOff();
	virtual void volumeUp();
	virtual void volumeDown();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	QString area;
	QString point;
};


class VirtualAmplifierDevice : public AmplifierDevice
{
Q_OBJECT
public:
	enum
	{
		REQ_AMPLI_ON = -2,       // set amplifier status: true = ON, false = OFF
		REQ_VOLUME_UP = 3,       // raise volume, value is the step
		REQ_VOLUME_DOWN = 4,     // decrease volume, value is the step
		REQ_SET_VOLUME = -1,     // set volume to specified level (range: 1-32)
	};

	VirtualAmplifierDevice(const QString &where, int openserver_id = 0);

	// must be called every time status/volume is set to inform other devices on the bus
	void updateStatus(bool status);
	void updateVolume(int volume);

	virtual void turnOn();
	virtual void turnOff();
	virtual void volumeUp();
	virtual void volumeDown();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};



/**
 * This class represent a device for managing the power amplifier, an evolved
 * version of the amplifier.
 */
class PowerAmplifierDevice : public AmplifierDevice
{
Q_OBJECT
public:
	PowerAmplifierDevice(QString address, int openserver_id = 0);

	virtual void init();

	void requestTreble() const;
	void requestBass() const;
	void requestBalance() const;
	void requestPreset() const;
	void requestLoud() const;

	enum
	{
		DIM_TREBLE = 2,
		DIM_BASS = 4,
		DIM_BALANCE = 17,
		DIM_PRESET = 19,
		DIM_LOUD = 20
	};

	void trebleUp() const;
	void trebleDown() const;
	void bassUp() const;
	void bassDown() const;
	void balanceUp() const;
	void balanceDown() const;
	void nextPreset() const;
	void prevPreset() const;
	void loudOn() const;
	void loudOff() const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};

#endif // MEDIA_DEVICE_H
