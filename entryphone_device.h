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
 * [TODO: are there any other cases? AUTOSWITCHING maybe?]
 * In this state, the device must check the 'where' field of incoming frames with its own address.
 */

class EntryphoneDevice : public device
{
friend class TestEntryphoneDevice;
Q_OBJECT
public:
	enum Type
	{
		CALL = 1,
		END_OF_CALL = 3,
	};
	// WARNING: this where has already indications about internal/external address
	// ie. it must be "1[conf_where]" for internal units and "2[conf_where]" for external units
	EntryphoneDevice(const QString &where);

	void answerCall() const;
	void endCall();
	virtual void manageFrame(OpenMsg &msg);
	void initVctProcess();

public slots:
	void cameraOn(QString _where) const;
	void stairLightActivate() const;
	void stairLightRelease() const;
	void openLock() const;
	void releaseLock() const;
	void cycleExternalUnits() const;
	void internalIntercomCall(QString _where) const; // an intercom between devices among the same SCS bus.
	void externalIntercomCall(QString _where) const; // an intercom between devices in different SCS bus.

private:
	void resetCallState();

	// call parameters
	int kind, mmtype;
	QString caller_address;
	bool is_calling;
};

#endif //ENTRYPHONE_DEVICE_H
