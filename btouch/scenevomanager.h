#ifndef SCENEVOMANAGER_H
#define SCENEVOMANAGER_H

#include "page.h"

class ScenEvoTimeCondition;
class ScenEvoDeviceCondition;
class BtButton;
class QStackedLayout;

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

private slots:
	void prev();
	void next();
	void ok();

private:
	BtButton *next_button;
	QStackedLayout *conditions_stack;
	void manageNextButton();
};


#endif // SCENEVOMANAGER_H
