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
class QTimer;



// The device for the alarm clock when using the sound diffusion to reproduce
// music from a source in multiple areas.
class AlarmSoundDiffDevice : public device
{
friend class TestAlarmSoundDiffDevice;
Q_OBJECT
public:
	enum Type
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
	void setActiveArea(QString area);

	static void addSource(SourceDevice *dev, int source_id);
	static void addAmplifier(AmplifierDevice *dev, int address);

	void requestStation(int source);

private slots:
	void sourceValueReceived(const DeviceValues &values_list);
	void amplifierValueReceived(const DeviceValues &values_list);

private:
	void updateActiveSource(DeviceValues &values_list);

private:
	bool receive_frames;
	QString area;

	// the list of the sources used in sound diffusion
	static QHash<int, SourceDevice*> sources;
	// the list of the amplifiers used in sound diffusion
	static QHash<int, AmplifierDevice*> amplifiers;

	// A trick. used to connect all the sound diffusion devices to the slots
	// sourceValueReceived / amplifierValueReceived
	static AlarmSoundDiffDevice *alarm_device;
};


/*!
	\ingroup SoundDiffusion
	\brief Device to control an SCS source.

	Can be used to turn on/turn off the source on a sound diffusion area and go to
	previous/next track.

	\section SourceDevice-dimensions Dimensions
	\startdim
	\dim{DIM_TRACK,int,,Current track/memorized radio station.}
	\dim{DIM_AREAS_UPDATED,no value,,The source was turned on/off on some SCS area; see isActive().}
	\enddim
*/
class SourceDevice : public device
{
friend class TestSourceDevice;
friend class TestVirtualSourceDevice;
Q_OBJECT
public:
	/*!
		\refdim{SourceDevice}
	*/
	enum Type
	{
		DIM_STATUS = 12,
		DIM_TRACK = 6,
		REQ_NEXT_TRACK = 9,
		REQ_PREV_TRACK = 10,
		DIM_AREAS_UPDATED = -2,
	};

	/*!
		\brief Constructor
		\param source_id SCS source id (1-4), not the OpenWebNet where.
	*/
	SourceDevice(QString source_id, int openserver_id = 0);

	virtual void init();

	/*!
		\brief Returns the SCS id of the source (the value passed to the constructor, not the OpenWebNet where).
	*/
	QString getSourceId() const { return source_id; }

	/*!
		\brief Must be called during initialization to set the sound diffusion type.
	*/
	static void setIsMultichannel(bool is_multichannel);

public slots:
	/*!
		\brief Go to the next track/memorized radio station.
	*/
	virtual void nextTrack();

	/*!
		\brief Go to previous track/memorized radio station.
	*/
	virtual void prevTrack();

	/*!
		\brief Turn on the source on in the specified sound diffusion area.

		For monochannel sound diffusion, always use \c "0" for the area.  For multichannel sound
		diffusion use values from \c "1" to \c "8".
		You can send the command for all the areas (as a general turn on) using a null QString
		as argument.
	*/
	virtual void turnOn(QString area);

	/*!
		\brief Request the current track/memorized radio station.
	*/
	void requestTrack() const;

public:
	/*!
		\brief Returns true if the source is active in the specified sound diffusion area.

		For monochannel sound diffusion, always use \c "0" for the area.  For multichannel sound
		diffusion use values from \c "1" to \c "8".
	*/
	bool isActive(QString area) const;

	/*!
		\brief Returns true if the source is active on any sound diffusion area.
	*/
	bool isActive() const;

protected:
	/*!
		\brief Sound diffusione multimedia type.
	*/
	int mmtype;

	/*!
		\brief Sound diffusion source id (1-4).
	*/
	QString source_id;

	/*!
		\brief Sound diffusione areas the source is active in.
	*/
	QSet<QString> active_areas;

	/*!
		\brief Whether the installed sound diffusion system is monochannel or multichannel.
	*/
	static bool is_multichannel;

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	// parse status updates for other sources to update the list of areas this device is active on
	virtual bool parseFrameOtherSources(OpenMsg &msg, DeviceValues &values_list);
	// parse general commands coming from an amplifier (required for next)
	virtual bool parseFrameAmplifiers(OpenMsg &msg, DeviceValues &values_list);

private:
	void sendTurnOn(QString area);

private slots:
	void requestActiveAreas() const;

};


/*!
	\ingroup SoundDiffusion
	\brief Device to control the RDS radio.

	Can be used to tune the radio and to receive RDS updates. The radio can memorize
	up to 5 stations.

	\section RadioSourceDevice-dimensions Dimensions
	\startdim
	\dim{DIM_FREQUENCY,int,,Radio frequency (in MHz * 100).}
	\dim{DIM_RDS,QString,,The RDS radio text.}
	\enddim
*/
class RadioSourceDevice : public SourceDevice
{
friend class TestRadioSourceDevice;
Q_OBJECT
public:
	/*!
		\refdim{RadioSourceDevice}
	*/
	enum Type
	{
		DIM_FREQUENCY = 5,
		DIM_RDS = 10,
	};

	/*!
		\brief Constructor
		\param source_id SCS source id (1-4), not the OpenWebNet where.
	*/
	RadioSourceDevice(QString source_id, int openserver_id = 0);

	virtual void init();

	/*!
		\brief Whether the device must automatically send RDS text updates.
	*/
	bool rdsUpdates() const { return rds_updates; }

public slots:
	/*!
		\brief Change radio frequency.

		If \a value is empty, the radio uses automatic search to find the next radio station.

		If \a value is not empty, the frequency is moved down by \a value * 0.05 MHz.
	*/
	void frequenceUp(QString value = QString());

	/*!
		\brief Change radio frequency.

		If \a value is empty, the radio uses automatic search to find the next radio station.

		If \a value is not empty, the frequency is moved up by \a value * 0.05 MHz.
	*/
	void frequenceDown(QString value = QString());

	/*!
		\brief Store the current frequency in the specified station slot (1-5).
	*/
	void saveStation(QString station) const;

	/*!
		\brief Tune the radio to a saved station (1-5).
	*/
	void setStation(QString station) const;

	/*!
		\brief Request current radio frequency.
	*/
	void requestFrequency() const;

	/*!
		\brief Start sending automatic updates for the RDS text.
	*/
	void requestStartRDS();

	/*!
		\brief Stop sending automatic updates for the RDS text.
	*/
	void requestStopRDS();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private slots:
	void stopRDS() const;

private:
	int frequency;
	int update_count; // the number of items that requests for RDS updates
	bool rds_updates; // the status of RDS updates
	QTimer *stopping_timer;
};


/*!
	\ingroup SoundDiffusion
	\brief The local sound diffusion source.

	This device manages SCS commands sent from the GUI/SCS to the local source.

	\section VirtualSourceDevice-dimensions Dimensions
	\startdim
	\dim{REQ_SOURCE_ON,no value,,Command: turn on the local source.}
	\dim{REQ_SOURCE_OFF,no value,,Command: turn off the local source.}
	\dim{REQ_NEXT_TRACK,no value,,Command: go to next track.}
	\dim{REQ_PREV_TRACK,no value,,Command: go to previous track.}
	\dim{DIM_SELF_REQUEST,bool,,Flag value: when true the command was sent by the GUI; when false it was received from the SCS bus.}
	\enddim
*/
class VirtualSourceDevice : public SourceDevice
{
Q_OBJECT
public:
	/*!
		\refdim{VirtualSourceDevice}
	*/
	enum Type
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

	/*!
		\brief Constructs the frame that is sent during startup to initialize the sound diffusion system.
		\param is_multichannel Whether sound diffusion is monochannel or multichannel.
		\param source_addr The SCS source id (1-4) of the local source (not the OpenWebNet where).
		\param ampli_addr The SCS address of the local amplifier (not the OpenWebNet where).
	*/
	static QString createMediaInitFrame(bool is_multichannel, const QString &source_addr, const QString &ampli_addr);

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};


/*!
	\ingroup SoundDiffusion
	\brief Device to control an SCS amplifier.

	Can be used to turn on/turn off the amplifier and change its volume.

	\section AmplifierDevice-dimensions Dimensions
	\startdim
	\dim{DIM_STATUS,bool,,Device status.}
	\dim{DIM_VOLUME,int,1-31,Amplifer volume.}
	\enddim

	Instantiating the correct class for an amplifier device is tricky because the
	local amplifier must be handled separately even for environment/global amplfiers.

	After calling setVirtualAmplifierWhere(), createDevice() will always create
	the correct device instance for a given point-to-point, environment or general address.
*/
class AmplifierDevice : public device
{
friend class TestAlarmSoundDiffDevice;
friend class TestAmplifierDevice;
Q_OBJECT
public:
	/*!
		\refdim{AmplifierDevice}
	*/
	enum Type
	{
		DIM_STATUS = 12,
		DIM_VOLUME = 1,
	};

	/*!
		\brief Amplifier device factory

		Transparently handles device creation for normal amplifier
		devices (either SCS or virtual) and for general and area commands; it will
		automatically handle the virtual amplifier and add the created device to the
		device cache.

		The \a where parameter can be:
		- \<area>\<point> (ex. \c 18, \c 23) will create a device for a single amplifier
		- \#\<area> (ex. \#4) will create an area command
		- \c "0" will create a general command

		If the touchscreen is configured as an SCS amplifier, setVirtualAmplifierWhere()
		must be called before instantiating any amplifier.
	*/
	static AmplifierDevice *createDevice(const QString &where, int openserver_id = 0);

	/*!
		\brief Set the where address of the local amplifier.

		\sa createDevice()
	*/
	static void setVirtualAmplifierWhere(const QString &where);

	/*!
		\brief Must be called during initialization to set the sound diffusion type.
	*/
	static void setIsMultichannel(bool is_multichannel);

	virtual void init();

	/*!
		\brief Request amplifier on/off status.

		It should never be necessary to call this function explicitly.
	*/
	void requestStatus() const;

	/*!
		\brief Request amplifier volume.

		It should never be necessary to call this function explicitly.
	*/
	void requestVolume() const;

	/*!
		\brief Turn on the amplifier.
	*/
	virtual void turnOn();

	/*!
		\brief Turn on the amplifier.
	*/
	virtual void turnOff();

	/*!
		\brief Increase amplifier volume by 1 (up to 31).
	*/
	virtual void volumeUp();

	/*!
		\brief Decrease amplifier volume by 1 (down to 1).
	*/
	virtual void volumeDown();

	/*!
		\brief Set amplifier volume to the specified level (1-31).
	*/
	virtual void setVolume(int volume);

	/*!
		\brief True if this is a general address (\c 0).
	*/
	static bool isGeneralAddress(const QString &where);

	/*!
		\brief True if thsi is an area address (\c \#area).
	*/
	static bool isAreaAddress(const QString &where);

	/*!
		\brief Returns the area of a point-to-point address or area address.
	*/
	static QString getAmplifierArea(const QString &where);

	/*!
		\brief Returns the sound diffusion area for this device.
	*/
	QString getArea() const { return area; }

	/*!
		\brief Returns the sound diffusion point for this device inside the device area.
	*/
	QString getPoint() const { return point; }

protected:
	/*!
		\brief Constructor

		The \a where can be \c 0 for a general device, \c \#a for an area device
		of a 2-digit number for a point-to-point device.

		Note that an AmplifierDevice will never affect the local amplfier, use VirtualAmplifierDevice
		to control the local amplifier.
	*/
	AmplifierDevice(QString where, int openserver_id = 0);

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	/*!
		\brief Returns whether a frame must be interpreted by the device.
	*/
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


/*!
	\ingroup SoundDiffusion
	\brief The local sound diffusion amplifier.

	This device manages SCS commands sent from the GUI/SCS to the local amplifier.

	\section VirtualAmplifierDevice-dimensions Dimensions
	\startdim
	\dim{REQ_AMPLI_ON,bool,,Command to turn on/turn off the local amplifier.}
	\dim{REQ_VOLUME_UP,int,,Command to increase amplifier volume (up to 31).}
	\dim{REQ_VOLUME_DOWN,int,,Command to decrease amplifier volume (down to 1).}
	\dim{REQ_SET_VOLUME,int,1-31,Command to set amplifier level to the given value.}
	\dim{REQ_TEMPORARY_OFF,no value,,Switch off the local amplifier for 1 second.  Do not send notifications and react to other commands as if there was no state change.}
	\dim{DIM_SELF_REQUEST,bool,,Flag value: when true the command was sent by the GUI; when false it was received from the SCS bus.}
	\enddim
*/
class VirtualAmplifierDevice : public AmplifierDevice
{
Q_OBJECT
public:
	/*!
		\refdim{VirtualAmplifierDevice}
	*/
	enum Type
	{
		REQ_AMPLI_ON = -2,
		REQ_VOLUME_UP = 3,
		REQ_VOLUME_DOWN = 4,
		REQ_SET_VOLUME = -1,
		REQ_TEMPORARY_OFF = -3,
		DIM_SELF_REQUEST = -4,
	};

	/*!
		\brief Constructor

		The \a where can only be a 2-digit number, and it must match the where
		configured for the local amplifier.
	*/
	VirtualAmplifierDevice(const QString &where, int openserver_id = 0);

	virtual void init();

	/*!
		\brief Emit a status update that amplifier status changed.

		Sends a status update frame to the bus and emits a \a valueReceived() signal
		with dimensions \a DIM_STATUS and \a DIM_SELF_REQUEST.
	 */
	void updateStatus(bool status);

	/*!
		\brief Emit a status update that amplifier volume changed.

		Sends a status update frame to the bus and emits a \a valueReceived() signal
		with dimensions \a DIM_VOLUME and \a DIM_SELF_REQUEST.
	 */
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


/*!
	\ingroup SoundDiffusion
	\brief Send command to multiple amplifier devices.

	This class is instantiated by \a createAmplifier() if needed; there should be no need
	to use it directly.

	Global and environment commands must act on both kinds of amplifiers at the same time, but
	\a VirtualAmpliferDevice only controls the local amplifier and \a AmplifierDervice controls
	any amplifier except the local amplifier.  Instances of this class containing both an
	\a AmplifierDevice and a \a VirtualAmplifierDevice instance allow controlling both kinds of
	amplifier using a single device instance.

	This device does not have a where address and does not produce status updates.
 */
class CompositeAmplifierDevice : public AmplifierDevice
{
Q_OBJECT
public:
	/*!
		\brief Constructs a device controlling the devices contained in the list.
	 */
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


/*!
	\ingroup SoundDiffusion
	\brief Controls the power amplifier.

	Allows changing power amplifier equalization and setting it to one of the
	presets.

	\section PowerAmplifierDevice-dimensions Dimensions
	\startdim
	\dim{DIM_TREBLE,int,-10/+10,Treble equalization.}
	\dim{DIM_BASS,int,-10/+10,Bass equalization.}
	\dim{DIM_BALANCE,int,-10 full left to +10 full right,Balance value.}
	\dim{DIM_LOUD,bool,,Loudness flag.}
	\dim{DIM_PRESET,int,0-9: fixed presets; 10-19: custom presets,Current equalization preset.}
	\enddim
 */
class PowerAmplifierDevice : public AmplifierDevice
{
Q_OBJECT
public:
	/*!
		\brief Constructor
	 */
	PowerAmplifierDevice(QString address, int openserver_id = 0);

	virtual void init();

	/*!
		\brief Request amplifier treble equalization value.

		It should never be necessary to call this function explicitly.
	 */
	void requestTreble() const;

	/*!
		\brief Request amplifier bass equalization value.

		It should never be necessary to call this function explicitly.
	 */
	void requestBass() const;

	/*!
		\brief Request amplifier left/right balance value.

		It should never be necessary to call this function explicitly.
	 */
	void requestBalance() const;

	/*!
		\brief Request the current preset of the amplifier.

		It should never be necessary to call this function explicitly.
	 */
	void requestPreset() const;

	/*!
		\brief Request amplifier loudness on/off flag.

		It should never be necessary to call this function explicitly.
	 */
	void requestLoud() const;

	/*!
		\refdim{PowerAmplifierDevice}
	 */
	enum Type
	{
		DIM_TREBLE = 2,
		DIM_BASS = 4,
		DIM_BALANCE = 17,
		DIM_PRESET = 19,
		DIM_LOUD = 20
	};

	/*!
		\brief Increase treble equalization.
	 */
	void trebleUp() const;

	/*!
		\brief Decrease treble equalization.
	 */
	void trebleDown() const;

	/*!
		\brief Increase bass equalization.
	 */
	void bassUp() const;

	/*!
		\brief Decrease bass equalization.
	 */
	void bassDown() const;

	/*!
		\brief Move balance to the right.
	 */
	void balanceUp() const;

	/*!
		\brief Move balance to the left.
	 */
	void balanceDown() const;

	/*!
		\brief Switch to next preset equalization.
	 */
	void nextPreset() const;

	/*!
		\brief Switch to previous preset equalization.
	 */
	void prevPreset() const;

	/*!
		\brief Turn on loudness.
	 */
	void loudOn() const;

	/*!
		\brief Turn off loudness.
	 */
	void loudOff() const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};



/*!
	\ingroup Scenarios
	\ingroup Antintrusion
	\brief Receive status updates from auxiliary command channel.

	Receives status updates from technical alarms independently from
	burglar alarm central unit status.

	\section AuxDevice-dimensions Dimensions
	\startdim
	\dim{DIM_STATUS,bool,,Auxillary command status.}
	\enddim
 */
// TODO move this together with other antintrusion devices
class AuxDevice : public device
{
friend class TestAuxDevice;
Q_OBJECT
public:
	AuxDevice(QString address, int openserver_id = 0);

	virtual void init();

	/*!
		\refdim{AuxDevice}
	 */
	enum Type
	{
		DIM_STATUS // the value doesn't matter
	};

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	void requestStatus();
};

#endif // MEDIA_DEVICE_H
