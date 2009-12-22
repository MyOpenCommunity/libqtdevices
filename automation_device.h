#ifndef DEV_AUTOMATION_H
#define DEV_AUTOMATION_H

#include "pulldevice.h"

class OpenMsg;


class AutomationDevice : public PullDevice
{
friend class TestAutomationDevice;
public:
	enum Type
	{
		DIM_STOP = 0,
		DIM_UP = 1,
		DIM_DOWN = 2,
	};

	AutomationDevice(QString where, PullMode mode = PULL_UNKNOWN);
	void goUp();
	void goDown();
	void stop();
	void requestStatus();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual void requestPullStatus();
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
	virtual void frame_rx_handler(char *c);
};

#endif // DEV_AUTOMATION_H
