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


#ifndef ENTRYPHONE_DEVICE_H
#define ENTRYPHONE_DEVICE_H

#include "device.h"

class OpenMsg;

/*
 * Controls entryphone functionality.
 * This device has two states: not connected and connected.
 *
 * Connected state:
 * The device switches to the connected state when a CALL frame arrives; in this state,
 * the field 'where' in incoming frames contains the address of the sender, not the device own address.
 *
 * Unconnected state:
 * The device switches to unconnected state when an END_OF_CALL frame arrives, or the user refuses the call.
 * In this state, the device must check the 'where' field of incoming frames with its own address.
 */

class EntryphoneDevice : public device
{
friend class TestEntryphoneDevice;
Q_OBJECT
public:
	enum Type
	{
		VCT_CALL = 1,
		AUTO_VCT_CALL = 99, // the value doesn't matter
		INTERCOM_CALL = 100, // the value doesn't matter
		ANSWER_CALL = 2,
		END_OF_CALL = 3,
		RINGTONE = 200, // the value doesn't matter
		MOVING_CAMERA = 201, // the value doesn't matter
		// The caller address should be an implementation detail of the device.
		// However, this dirty klugde is required because for the professional
		// studio functionality we have to use the caller_address, obtained
		// from the homonymous frame.
		CALLER_ADDRESS = 9,
		STOP_VIDEO = 202, // the value doesn't matter
		VCT_TYPE,  // the value doesn't matter
	};

	enum VctType
	{
		AUDIO_VIDEO,
		ONLY_AUDIO
	};

	EntryphoneDevice(const QString &where, QString mode = QString(), int openserver_id = 0);

	void answerCall() const;
	void initVctProcess();
	virtual void init() { initVctProcess(); }

public slots:
	void endCall();
	void cameraOn(QString _where) const;
	void stairLightActivate() const;
	void stairLightRelease() const;
	void openLock() const;
	void releaseLock() const;
	void cycleExternalUnits() const;
	void internalIntercomCall(QString _where); // an intercom between devices among the same SCS bus.
	void externalIntercomCall(QString _where); // an intercom between devices in different SCS bus.
	// Movements
	void moveUpPress() const;
	void moveUpRelease() const;
	void moveDownPress() const;
	void moveDownRelease() const;
	void moveLeftPress() const;
	void moveLeftRelease() const;
	void moveRightPress() const;
	void moveRightRelease() const;

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
	QString vct_mode;
};

#endif //ENTRYPHONE_DEVICE_H
