#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include "page.h"
#include "main.h"
#include "datetime.h" //BtTimeEdit

#include <QTimer>


class BtButton;
class DeviceCondition;


/**
 * This class is an abstract class that represent an evolved scenario condition.
 */
class ScenEvoCondition : public Page
{
Q_OBJECT
public:
	//! A type flag, used because RTTI is disabled.
	bool hasTimeCondition;

	/*!
	\brief returns description of condition
	*/
	virtual const char *getDescription();
	//! Set serial number
	void set_serial_number(int);
	//! Get serial number
	int get_serial_number();
	//! Inits condition
	virtual void inizializza();
	//! Returns true when condition is satisfied
	virtual bool isTrue();

public slots:
	//! Next button pressed
	virtual void Next();
	//! Prev button pressed
	virtual void Prev();
	//! OK button pressed
	virtual void OK();
	//! Apply button pressed
	virtual void Apply();
	//! Save condition
	virtual void save();
	//! Reset condition (on cancel)
	virtual void reset();

signals:
	//! Emitted when user clicks Next icon
	void SwitchToNext();
	//! Emitted when user clicks Prev icon (NO MORE !!!)
	void SwitchToPrev();
	//! Emitted when user ckicks Prev icon
	void SwitchToFirst();
	//! Emitted when condition is true
	void verificata();
	//! Emitted when all conditions in list must be reset
	void resetAll();
	//! Emitted when all conditions in list are OK (must be saved and applied)
	void okAll();
	//! After a status changed, a condition is satisfied
	void condSatisfied();

protected:
	int item_id;

	// The constructor is protected to avoid the building of ScenEvoCondition objects.
	ScenEvoCondition();

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
	/*!
	\brief Returns condition description
	*/
	const char *getDescription();

	//! Save condition
	void save();
	//! Return true when condition is satisfied
	bool isTrue();
public slots:
	//! OK method
	void OK();
	//! Apply method
	void Apply();
	//! Reset condition
	void reset();
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

	/*!
	\brief Returns condition description in human language
	*/
	const char *getDescription();
	//! Save condition
	virtual void save();
	//! Return true when condition is satisfied
	bool isTrue();

public slots:
	//! OK method
	void OK();
	//!  Apply method
	void Apply();
	//! Reset condition
	void reset();

private:
	//! Specific device condition
	DeviceCondition *device_cond;
	//! Inits condition
	void inizializza();
};

#endif // _SCENEVOCOND_H_
