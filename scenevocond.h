#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include "page.h"
#include "main.h"
#include "datetime.h" //BtTimeEdit

#include <QTimer>


class BtButton;
class DeviceCondition;
class QStackedLayout;

/**
 * This class is an abstract page that represent an evolved scenario condition.
 */
class ScenEvoCondition : public QWidget
{
Q_OBJECT
public:
	void set_serial_number(int);
	int get_serial_number();

public slots:
	virtual void Apply() = 0;
	virtual void save() = 0;
	//! Reset condition (on cancel)
	virtual void reset() = 0;

signals:
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
	ScenEvoTimeCondition(int item_id, const QDomNode &config_node);

public slots:
	virtual void save();
	virtual void Apply();
	virtual void reset();

private slots:
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

	virtual void inizializza();
	//! Returns true when the condition is satisfied
	virtual bool isTrue();

public slots:
	virtual void save();
	virtual void Apply();
	virtual void reset();

private:
	//! Specific device condition
	DeviceCondition *device_cond;
};


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

#endif // _SCENEVOCOND_H_
