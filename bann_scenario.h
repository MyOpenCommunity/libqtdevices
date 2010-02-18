#ifndef BANN_SCENARIO_H
#define BANN_SCENARIO_H

#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // Bann3Buttons
#include "bann4_buttons.h" // Bann4ButtonsIcon, bann4But
#include "device.h" // StatusList

#include <QList>

class PPTSceDevice;
class ScenEvoCondition;
class ScenEvoTimeCondition;
class ScenEvoDeviceCondition;
class device;
class QTimerEvent;
class QWidget;
class QDomNode;
class ScenarioDevice;



class BannSimpleScenario : public Bann2Buttons
{
Q_OBJECT
public:
	BannSimpleScenario(int scenario, const QString &descr, const QString &where);

private slots:
	void activate();

private:
	ScenarioDevice *dev;
	int scenario_number;
};


class ScenarioModule : public Bann4ButtonsIcon
{
Q_OBJECT
public:
	ScenarioModule(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza = false);

private slots:
	void status_changed(const StatusList &sl);
	void activate();
	void editScenario();
	void startEditing();
	void deleteScenario();
	void stopEditing();

private:
	void changeLeftFunction(const char *slot);
	ScenarioDevice *dev;
	int scenario_number;
	bool is_editing;
};


/**
 * This banner represent an evolved scenario, that can do actions based on devices
 * and times based conditions.
 */
class ScenarioEvolved : public Bann3Buttons
{
Q_OBJECT
public:
	ScenarioEvolved(int _item_id, QString descr, QString _action, bool _enabled,
		ScenEvoTimeCondition *tcond, ScenEvoDeviceCondition *dcond);

public slots:
	void inizializza(bool forza = false);

private slots:
	void toggleActivation();
	void forceTrig();
	void trig(bool forced = false);
	void trigOnStatusChanged();

	void reset();
	void save();

private:
	QString action, enable_icon, disable_icon;
	int serial_number;
	static int next_serial_number;
	bool enabled;
	int item_id;
	ScenEvoTimeCondition *time_cond;
	ScenEvoDeviceCondition *device_cond;
};


class ScheduledScenario : public Bann4Buttons
{
Q_OBJECT
public:
	ScheduledScenario(QWidget *parent, const QDomNode &config_node);

private slots:
	void enable();
	void disable();
	void start();
	void stop();

private:
	QString action_enable, action_disable, action_start, action_stop;
};


class PPTSce : public bann4But
{
Q_OBJECT
public:
	PPTSce(QWidget *parent, QString where, int cid);

protected:
	virtual void timerEvent(QTimerEvent *e);

private:
	PPTSceDevice *dev;
	int increase_timer, decrease_timer;

private slots:
	void startIncrease();
	void startDecrease();
	void stop();
};

#endif
