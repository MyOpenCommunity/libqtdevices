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


#ifndef VIDEODOOR_ENTRY_DEVICE_H
#define VIDEODOOR_ENTRY_DEVICE_H

#include "device.h"

class OpenMsg;
class QString;


/*!
	\ingroup VideoDoorEntry
	\brief Controls Video Door Entry functionalities for ts 3.5''.

	This device is a simplified version of the VideoDoorEntryDevice, that can be
	used on the touchscreen 3.5'' where only a subset of the video door entry
	functionalities are exposed by the openserver.

	It notifies incoming calls emitting the valueReceived() signal as usual,
	and can be used to controls the lock door using the methods openLock() and
	releaseLock() and to switch on/off the stair light using the methods
	stairLightActivate() and stairLightRelease().

	\section BasicVideoDoorEntryDevice-dimensions Dimensions
	\startdim
	\dim{CALLER_ADDRESS,QString,,The address of the caller.}
	\enddim
*/
class BasicVideoDoorEntryDevice : public device
{
Q_OBJECT
public:

	/*!
		\refdim{BasicVideoDoorEntryDevice}
	*/
	enum Type
	{
		CALLER_ADDRESS = 9,
	};

	/*!
		\brief The modality of the video door entry system.
	*/
	enum VctMode
	{
		NONE = 0,
		SCS_MODE, /*!< For scs videocalls */
		IP_MODE   /*!< For ip videocalls */
	};

	BasicVideoDoorEntryDevice(const QString &where, QString mode = QString(), int openserver_id = 0);

	/*!
		\brief Initialize the Video Door Entry underlying system.

		It should never be necessary to call this function explicitly.
	*/
	void initVctProcess();

	/*!
		\brief Return the modality of the Video Door Entry system.
	*/
	VctMode vctMode() const { return vct_mode; }

public slots:
	/*!
		\brief Switch on the stairlight.
	*/
	void stairLightActivate(QString target_where) const;

	/*!
		\brief Switch off the stairlight.
	*/
	void stairLightRelease(QString target_where) const;

	/*!
		\brief Activate a lock actuator.
	*/
	void openLock(QString target_where) const;

	/*!
		\brief Release a lock actuator.
	*/
	void releaseLock(QString target_where) const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	VctMode vct_mode;
};


/*!
	\ingroup VideoDoorEntry
	\brief Controls Video Door Entry functionalities.

	This device can be used to control a Video Door Entry call or a Video Door Entry
	actuator. While the latter is pretty simple, a Video Door Entry call can be
	only Audio or AudioVideo (see VctType) and also SCS or IP (see VctMode).

	Because not all of these differences can be abstracted (for example: in
	the SCS videocall the process that captures the video must be handled manually
	while in the IP videocall the same process is handled by the underlying
	video process) this device provides info about the type through the
	dimension VCT_TYPE and the mode through the methods ipCall() and vctMode().

	The device can be used to switch on an external camera using the cameraOn()
	method, or perform an intercom call using the methods internalIntercomCall()
	and externalIntercomCall().
	There are also several methods to control the movements of the camera, open
	the door lock and switch on/off the staircase light.

	\section VideoDoorEntryDevice-dimensions Dimensions
	\startdim
	\dim{VCT_CALL,VctType,,An incoming call.}
	\dim{AUTO_VCT_CALL,VctType,,An incoming call caused by an autoswitch on of a camera.}
	\dim{INTERCOM_CALL,VctType,,An incoming intercom call.}
	\dim{ANSWER_CALL,,,Used only in the intercom calls\, when the other side of the communication answer to the call.}
	\dim{END_OF_CALL,,,The other side has closed the call.}
	\dim{RINGTONE,Ringtone,,The type of the ringtone to play.}
	\dim{MOVING_CAMERA,bool,,True if the camera support movements. }
	\dim{CALLER_ADDRESS,QString,,The address of the caller. An address prepended by the symbol @ means that it comes from an autoswitch call. }
	\dim{STOP_VIDEO,bool,,Stop the reproduction of the video.}
	\dim{VCT_TYPE,VctType,,The type of the call.}
	\dim{RESTORE_MM_AMPLI,,,Restore the local amplifier if previously silenced.}
	\dim{SILENCE_MM_AMPLI,,,Silence the local amplifier if present.}
	\enddim

	\internal
	The device has two states: not connected and connected.

	Connected state:
	The device switches to the connected state when a CALL frame arrives; in
	this state, the field 'where' in incoming frames contains the address of
	the sender, not the device own address.

	Unconnected state:
	The device switches to unconnected state when an END_OF_CALL frame arrives, or
	the user refuses the call.
	In this state, the device must check the 'where' field of incoming frames
	with its own address.
*/
class VideoDoorEntryDevice : public BasicVideoDoorEntryDevice
{
friend class TestVideoDoorEntryDevice;
Q_OBJECT
public:
	/*!
		\refdim{VideoDoorEntryDevice}
	*/
	enum Type
	{
		VCT_CALL = 1,
		AUTO_VCT_CALL = 99, // the value doesn't matter
		INTERCOM_CALL = 100, // the value doesn't matter
		PAGER_CALL = 101, // the value doesn't matter
		ANSWER_CALL = 2,
		END_OF_CALL = 3,
		RINGTONE = 200, // the value doesn't matter
		MOVING_CAMERA = 201, // the value doesn't matter
		CALLER_ADDRESS = 9,
		STOP_VIDEO = 202, // the value doesn't matter
		VCT_TYPE,  // the value doesn't matter
		RESTORE_MM_AMPLI = 64,
		SILENCE_MM_AMPLI = 63,
	};

	/*!
		\brief The types of ringtones.

		A ringtone type defines a list of logical event for which a ringtone should
		be played.
	*/
	enum Ringtone
	{
		// for old TS 3.5/TS 10 code, this must match the values in ringtonesmanager.h
		PE1 = 1,
		PE2,
		PE3,
		PE4,
		PI_INTERCOM,
		PE_INTERCOM,
		FLOORCALL
	};

	/*!
		\brief The videocall type.
	*/
	enum VctType
	{
		AUDIO_VIDEO, /*!< Audio-video call. */
		ONLY_AUDIO   /*!< Audio call */
	};

	/*!
		\brief Constructor.

		The \a where parameter can be the address of the touchscreen or the address of
		an actuator/entrance panel, the \a mode parameter reflects one of the values in
		the VctMode enum: a null QString means NONE, "1" means IP_MODE and everything
		else stands for SCS_MODE.
	*/
	VideoDoorEntryDevice(const QString &where, QString mode = QString(), int openserver_id = 0);

	/*!
		\brief Accept an incoming call.
	*/
	void answerCall() const;

	/*!
		\brief Return true if the call is an ip one.
		\sa VctMode
	*/
	bool ipCall() const { return ip_call; }

public slots:
	/*!
		\brief Close a call.
	*/
	void endCall();

	/*!
		\brief Turn on a camera.

		When the connection is established a signal valueReceived() containing
		the dimension AUTO_VCT_CALL is emitted.
	*/
	void cameraOn(QString where) const;

	/*!
		\brief Switch on the stairlight.
	*/
	void stairLightActivate() const;

	/*!
		\brief Switch off the stairlight.
	*/
	void stairLightRelease() const;

	/*!
		\brief Activate a lock actuator.
	*/
	void openLock() const;

	/*!
		\brief Release a lock actuator.
	*/
	void releaseLock() const;

	/*!
		\brief Switch from a camera to another one.

		Change the current camera watched. After this command, a valueReceived()
		containing the dimensions VCT_TYPE and MOVING_CAMERA is emitted.
		\sa cameraOn()
	*/
	void cycleExternalUnits() const;

	/*!
		\brief Perform an intercom call between devices among the same SCS bus.
	*/
	void internalIntercomCall(QString where);

	/*!
		\brief Perform an intercom call between devices in different SCS bus.
	*/
	void externalIntercomCall(QString where);

	/*!
		\brief If the camera support movements, start moving up the camera.
	*/
	void moveUpPress() const;

	/*!
		\brief If the camera support movements, stop moving up the camera.
	*/
	void moveUpRelease() const;

	/*!
		\brief If the camera support movements, start moving down the camera.
	*/
	void moveDownPress() const;

	/*!
		\brief If the camera support movements, stop moving down the camera.
	*/
	void moveDownRelease() const;

	/*!
		\brief If the camera support movements, start moving left the camera.
	*/
	void moveLeftPress() const;

	/*!
		\brief If the camera support movements, stop moving left the camera.
	*/
	void moveLeftRelease() const;

	/*!
		\brief If the camera support movements, start moving right the camera.
	*/
	void moveRightPress() const;

	/*!
		\brief If the camera support movements, stop moving right the camera.
	*/
	void moveRightRelease() const;

	/*!
		\brief Starts a pager call.
	*/
	void pagerCall();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	void resetCallState();
	void cameraMovePress(int move_type) const;
	void cameraMoveRelease(int move_type) const;

	// call parameters
	int kind, mmtype;
	QString caller_address;
	QString master_caller_address;
	bool is_calling;
	bool ip_call;
};

#endif //VIDEODOOR_ENTRY_DEVICE_H
