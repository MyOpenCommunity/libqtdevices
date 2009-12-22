#ifndef SCENARIO_DEVICE_H
#define SCENARIO_DEVICE_H

#include "device.h"

class OpenMsg;

typedef QPair<bool, int> ScenarioProgrammingStatus;
Q_DECLARE_METATYPE(ScenarioProgrammingStatus);

class ScenarioDevice : public device
{
friend class TestScenarioDevice;
public:
	enum Type
	{
		DIM_START,     // scenario programming start (false when stops)
		DIM_LOCK,      // device lock enabled (false when unlock)
	};

	ScenarioDevice(QString where);

	void activateScenario(int scen);
	void startProgramming(int scen);
	void stopProgramming(int scen);
	void deleteAll();
	void deleteScenario(int scen);

	void requestStatus();
	// TODO: we shouldn't need lock/unlock methods, the GUI can't lock

	// TODO: hack to avoid too many changes in device.h, REMOVE when new device parsing is ok.
	virtual void frame_rx_handler(char *frame);
	virtual void manageFrame(OpenMsg &msg);
};


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

#endif // SCENARIO_DEVICE_H
