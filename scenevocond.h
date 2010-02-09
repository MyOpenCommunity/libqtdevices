#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include "page.h"
#include "main.h"
#include "datetime.h" //BtTimeEdit

#include <QTimer>


class BtButton;
class DeviceCondition;


/**
 * This class is an abstract page that represent an evolved scenario condition.
 */
class ScenEvoCondition : public Page
{
Q_OBJECT
public:
	virtual const char *getDescription() = 0;
	void set_serial_number(int);
	int get_serial_number();

	//! Returns true when condition is satisfied
	virtual bool isTrue() = 0;

public slots:
	virtual void Next();
	virtual void Prev();
	virtual void OK();

	virtual void Apply() = 0;
	virtual void save() = 0;
	//! Reset condition (on cancel)
	virtual void reset() = 0;

signals:
	//! Emitted when user clicks Next icon
	void SwitchToNext();
	//! Emitted when user clicks Prev icon (NO MORE !!!)
	void SwitchToPrev();
	//! Emitted when user ckicks Prev icon
	void SwitchToFirst();
	//! Emitted when all conditions in list must be reset
	void resetAll();
	//! Emitted when all conditions in list are OK (must be saved and applied)
	void okAll();
	//! After a status changed, a condition is satisfied
	void condSatisfied();

protected:
	int item_id;

private:
	int serial_number;
};


/**
 * This class represent a time based evolved scenario condition.
 */
class ScenEvoTimeCondition : public ScenEvoCondition
{
Q_OBJECT
public:
	ScenEvoTimeCondition(int item_id, const QDomNode &config_node, bool has_next);
	virtual const char *getDescription();
	virtual bool isTrue();

public slots:
	virtual void OK();
	virtual void save();
	virtual void Apply();
	virtual void reset();
	//! Timer expired method
	void scaduta();
	//! Just setup qt timer (based on cond_time)
	void setupTimer();

private:
	QTime cond_time;
	BtButton *bottom_left, *bottom_center, *bottom_right;

	BtTimeEdit time_edit;
	QTimer timer;
};


/**
 * This class represent a device based evolved scenario condition.
 */
class ScenEvoDeviceCondition : public ScenEvoCondition
{
Q_OBJECT
public:
	ScenEvoDeviceCondition(int item_id, const QDomNode &config_node);
	~ScenEvoDeviceCondition();

	virtual const char *getDescription();
	virtual bool isTrue();
	virtual void inizializza();

public slots:
	virtual void OK();
	virtual void save();
	virtual void Apply();
	virtual void reset();

private:
	//! Specific device condition
	DeviceCondition *device_cond;
};

#endif // _SCENEVOCOND_H_
