#ifndef SCENARIO_DEVICE_H
#define SCENARIO_DEVICE_H

#include "device.h"

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

	virtual void manageFrame(OpenMsg &msg);
};

#endif // SCENARIO_DEVICE_H
