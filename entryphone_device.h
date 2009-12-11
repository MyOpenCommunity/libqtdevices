#ifndef ENTRYPHONE_DEVICE_H
#define ENTRYPHONE_DEVICE_H

#include "device.h"

class OpenMsg;

class EntryphoneDevice : public device
{
friend class TestEntryphoneDevice;
Q_OBJECT
public:
	enum Type
	{
		INCOMING_CALL = 1,
		END_OF_CALL = 3,
	};
	// WARNING: this where has already indications about internal/external address
	// ie. it must be "1[conf_where]" for internal units and "2[conf_where]" for external units
	EntryphoneDevice(const QString &where);

	void answerCall() const;
	void endCall() const;
	// used to light a video camera in video control
	void autoSwitching() const;
	// aka CameraSliding
	void cycleCamera() const;

	virtual void manageFrame(OpenMsg &msg);

private:
	// call parameters
	int kind, mmtype;
};

#endif //ENTRYPHONE_DEVICE_H
