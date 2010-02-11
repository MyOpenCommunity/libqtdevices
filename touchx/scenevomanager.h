#ifndef SCENEVOMANAGER_H
#define SCENEVOMANAGER_H

#include "page.h"

class ScenEvoTimeCondition;
class ScenEvoDeviceCondition;


/**
 * The graphical manager for the evolved scenarios page.
 */
class ScenEvoManager : public Page
{
Q_OBJECT
public:
	ScenEvoManager(ScenEvoTimeCondition *time_cond, ScenEvoDeviceCondition *device_cond);

signals:
	void reset();
	void save();
};


#endif // SCENEVOMANAGER_H
