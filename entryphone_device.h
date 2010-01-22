#ifndef ENTRYPHONE_DEVICE_H
#define ENTRYPHONE_DEVICE_H

#include "device.h"

class OpenMsg;

/*
 * Controls entryphone functionality. There can be only one instance of this class, since Touch 10 is only
 * one destination for calls.
 * This device has two states: not connected and connected.
 *
 * Connected state:
 * The device switces to the connected state when a CALL frame arrives; in this state,
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
		INTERCOM_CALL = 100, // the value doesn't matter
		END_OF_CALL = 3,
		RINGTONE = 200, // the value doesn't matter
		MOVING_CAMERA = 201, // the value doesn't matter
	};

	EntryphoneDevice(const QString &where);

	void answerCall() const;
	virtual void manageFrame(OpenMsg &msg);
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

private:
	void resetCallState();
	void cameraMovePress(int move_type) const;
	void cameraMoveRelease(int move_type) const;

	// call parameters
	int kind, mmtype;
	QString caller_address;
	bool is_calling;
};

#endif //ENTRYPHONE_DEVICE_H
