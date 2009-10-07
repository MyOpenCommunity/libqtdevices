#ifndef SCENARIO_DEVICE_H
#define SCENARIO_DEVICE_H

#include "device.h"

class ScenarioDevice : public device
{
public:
	ScenarioDevice(QString where);

	void activateScenario(int scen);
	void startProgramming(int scen);
	void stopProgramming(int scen);
	void deleteAll();
	void deleteScenario(int scen);
	// TODO: we shouldn't need lock/unlock methods, the GUI can't lock

	virtual void manageFrame(OpenMsg &msg);
};

#endif // SCENARIO_DEVICE_H
