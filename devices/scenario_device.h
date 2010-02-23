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
		DIM_START,     // scenario programming start (false when stops). A DIM_START update means
		               // the device is also unlocked.
		DIM_LOCK,      // device lock enabled (false when unlock)
	};

	enum
	{
		ALL_SCENARIOS = -1,     // all scenarios (dummy value, must be different from legal values)
	};

	ScenarioDevice(QString where, int openserver_id = 0);

	void activateScenario(int scen);
	void startProgramming(int scen);
	void stopProgramming(int scen);
	void deleteAll();
	void deleteScenario(int scen);

	void requestStatus();

	virtual void manageFrame(OpenMsg &msg);

private:
	bool is_unlocked;
};


class PPTSceDevice : public device
{
Q_OBJECT
public:
	PPTSceDevice(QString address, int openserver_id = 0);
	void increase() const;
	void decrease() const;
	void stop() const;

public slots:
	void turnOn() const;
	void turnOff() const;
};

#endif // SCENARIO_DEVICE_H
