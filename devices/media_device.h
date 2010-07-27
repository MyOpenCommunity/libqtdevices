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
#include <QHash>

class SourceDevice;
class AmplifierDevice;


/**
 * The device for the alarm clock when using the sound diffusion to reproduce
 * music from a source in multiple areas.
 */
class AlarmSoundDiffDevice : public device
{
friend class TestAlarmSoundDiffDevice;
Q_OBJECT
public:
	enum
	{
		DIM_AMPLIFIER,
		DIM_STATUS,
		DIM_VOLUME,
		DIM_SOURCE,
		DIM_RADIO_STATION
	};

	AlarmSoundDiffDevice();

	void startAlarm(bool is_multichannel, int source, int radio_station, int *alarmVolumes);
	bool isValid(int source, int radio_station, int *alarmVolumes);
	void stopAlarm(int source, int *alarmVolumes);
	void setVolume(int amplifier, int volume);

	// The device for the alarm clock (that uses the sound diffusion) has a
	// different logic than the other devices:
	// it registers the status of the sound diffusion during the configuration
	// to discover what is the active source (and the station if is a radio)
	// and what are the active amplifiers in order to use the same setup of the
	// sound diffusion during the alarm.
	void setReceiveFrames(bool receive);

	static void addSource(SourceDevice *dev, int source_id);
	static void addAmplifier(AmplifierDevice *dev, int address);

	void requestStation(int source);

private slots:
	void sourceValueReceived(const DeviceValues &values_list);
	void amplifierValueReceived(const DeviceValues &values_list);

private:
	bool receive_frames;
	// the list of the sources used in sound diffusion
	static QHash<int, SourceDevice*> sources;
	// the list of the amplifiers used in sound diffusion
	static QHash<int, AmplifierDevice*> amplifiers;

	// A trick. used to connect all the sound diffusion devices to the slots
	// sourceValueReceived / amplifierValueReceived
	static AlarmSoundDiffDevice *alarm_device;
};


/**
 * This class implements a multimedia source device, that can be used to send
 * command to the actual device or notified status changed.
 */
class SourceDevice : public device
{
friend class TestSourceDevice;
friend class TestVirtualSourceDevice;
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

	virtual void init();
	QString getSourceId() const { return source_id; }

public slots:
	virtual void nextTrack();
	virtual void prevTrack();
	virtual void turnOn(QString area);
	void requestTrack() const;

public:
	bool isActive(QString area) const;
	bool isActive() const;

protected:
	int mmtype;
	QString source_id;
	QSet<QString> active_areas;

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	// parse status updates for other sources to update the list of areas this device is active on
	virtual bool parseFrameOtherSources(OpenMsg &msg, DeviceValues &values_list);
	// parse general commands coming from an amplifier (required for next)
	virtual bool parseFrameAmplifiers(OpenMsg &msg, DeviceValues &values_list);

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

	virtual void init();

public slots:
	void frequenceUp(QString value = QString());
	void frequenceDown(QString value = QString());
	void saveStation(QString station) const;
	void setStation(QString station) const;

	void requestFrequency() const;
	void requestRDS() const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	int frequency;
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
		REQ_SOURCE_ON = 1,
		REQ_SOURCE_OFF,
		// boolean, set to true if the status update is from a command we sent ourselves
		DIM_SELF_REQUEST,
	};

	VirtualSourceDevice(QString address, int openserver_id = 0);

	virtual void nextTrack();
	virtual void prevTrack();
	virtual void turnOn(QString area);

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
friend class TestAlarmSoundDiffDevice;
friend class TestAmplifierDevice;
Q_OBJECT
public:
	enum
	{
		DIM_STATUS = 12,
		DIM_VOLUME = 1,
	};

	/*
	 * this function transparently handles device creation for normal amplifier
	 * devices (either SCS or virtual) and for general and area commands; it will
	 * automatically handle the virtual amplifier and add the created device to the
	 * device cache.
	 *
	 * passing <area><point> as the where (eg. 18, 23) will create a device for a single amplifier
	 * passing #<area> as the where (es. #4) will create an area command
	 * passing "0" as the where will create a general command
	 */
	static AmplifierDevice *createDevice(const QString &where);

	static void setVirtualAmplifierWhere(const QString &where);
	static void setIsMultichannel(bool is_multichannel);

	virtual void init();

	void requestStatus() const;
	void requestVolume() const;
	virtual void turnOn();
	virtual void turnOff();
	virtual void volumeUp();
	virtual void volumeDown();
	virtual void setVolume(int volume);

	static bool isGeneralAddress(const QString &where);
	static bool isAreaAddress(const QString &where);
	static QString getAmplifierArea(const QString &where);
	QString getArea() const { return area; }
	QString getPoint() const { return point; }

protected:
	// passing "0" as the where creates a device commanding all amplifiers
	// passing "#<area>" as the where creates a device commanding all amplifiers in an area
	AmplifierDevice(QString where, int openserver_id = 0);

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	bool checkAddressIsForMe(OpenMsg &msg);

private:
	static AmplifierDevice *createVirtualDevice();

protected:
	QString area;
	QString point;
	static bool is_multichannel;

private:
	static QString virtual_amplifier_where;
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
		REQ_TEMPORARY_OFF = -3,  // temporary off of the amplifier
		// boolean, set to true if the status update is from a command we sent ourselves
		DIM_SELF_REQUEST = -4,
	};

	VirtualAmplifierDevice(const QString &where, int openserver_id = 0);

	virtual void init();

	// must be called every time status/volume is set to inform other devices on the bus
	void updateStatus(bool status);
	void updateVolume(int volume);

	virtual void turnOn();
	virtual void turnOff();
	virtual void volumeUp();
	virtual void volumeDown();
	virtual void setVolume(int volume);

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	bool status;
	int volume;
};


/**
 * This class represent a group of amplifiers.  It is used to send general/environment
 * command to both normal SCS amplifiers and a virtual amplifier at the same time.
 */
class CompositeAmplifierDevice : public AmplifierDevice
{
Q_OBJECT
public:
	CompositeAmplifierDevice(const QList<AmplifierDevice*> &devices);

	virtual QString get_key();

	virtual void turnOn();
	virtual void turnOff();
	virtual void volumeUp();
	virtual void volumeDown();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	QList<AmplifierDevice*> devices;
	QString key;
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



/**
 * The device for the aux
 */
class AuxDevice : public device
{
friend class TestAuxDevice;
Q_OBJECT
public:
	AuxDevice(QString address, int openserver_id = 0);

	virtual void init();

	enum
	{
		DIM_STATUS // the value doesn't matter
	};

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	void requestStatus();
};

#endif // MEDIA_DEVICE_H
