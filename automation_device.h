#ifndef DEV_AUTOMATION_H
#define DEV_AUTOMATION_H

#include "device.h"

class OpenMsg;


class AutomationDevice : public device
{
friend class TestAutomationDevice;
public:
	enum PullMode
	{
		PULL,
		NOT_PULL,
		PULL_UNKNOWN,
	};

	enum Type
	{
		DIM_STOP = 0,
		DIM_UP = 1,
		DIM_DOWN = 2,
	};

	AutomationDevice(QString where, PullMode mode = PULL);
	void goUp();
	void goDown();
	void stop();
	void requestStatus();

	virtual void manageFrame(OpenMsg &msg);
private:
	PullMode mode;
};

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
