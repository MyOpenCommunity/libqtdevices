#ifndef DEV_AUTOMATION_H
#define DEV_AUTOMATION_H

#include "device.h"

class OpenMsg;


/**
 * \class PPTStatDevice
 *
 * This class represent a device for managing the PPTStat
 */
class PPTStatDevice : public device
{
Q_OBJECT
public:
	PPTStatDevice(QString address);
	void requestStatus() const;

	// This enum is required to pass the information about the status with the status_changed signal
	enum Type
	{
		DIM_STATUS
	};

	virtual void manageFrame(OpenMsg &msg);
};


// TODO: move in a separate file! (dev_scenario?)
class PPTSceDevice : public device
{
Q_OBJECT
public:
	PPTSceDevice(QString address);
	void increase() const;
	void decrease() const;
	void stop() const;

public slots:
	void turnOn() const;
	void turnOff() const;
};

#endif // DEV_AUTOMATION_H
