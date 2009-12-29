#ifndef BANN_SCENARIO_H
#define BANN_SCENARIO_H

#include "bann1_button.h" // BannLeft
#include "bann3_buttons.h" // Bann3Buttons
#include "bann4_buttons.h" // Bann4ButtonsIcon, bann4But
#include "device.h" // StatusList

#include <QList>

class PPTSceDevice;
class scenEvo_cond;
class device;
class QTimerEvent;
class QWidget;
class QDomNode;
class ScenarioDevice;



class BannSimpleScenario : public BannLeft
{
Q_OBJECT
public:
	BannSimpleScenario(QWidget *parent, const QDomNode &config_node);

private slots:
	void activate();

private:
	ScenarioDevice *dev;
	int scenario_number;
};


class ModifyScenario : public Bann4ButtonsIcon
{
Q_OBJECT
public:
	ModifyScenario(QWidget *parent, const QDomNode &config_node);
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


/*!
 * \class scenEvo
 * \brief This class represents an advanced scenario management object
 * \author Ciminaghi
 * \date apr 2006
 */
class scenEvo : public Bann3Buttons
{
Q_OBJECT
public:
	scenEvo(QWidget *parent, const QDomNode &conf_node, QList<scenEvo_cond*> c);
	~scenEvo();
public slots:
	void inizializza(bool forza = false);

private:
	QList<scenEvo_cond*> condList;
	unsigned current_condition;
	QString action, enable_icon, disable_icon;
	int serial_number;
	static int next_serial_number;
	bool enabled;

private slots:
	void toggleAttivaScev();
	void configScev();
	void forzaScev();
	void nextCond();
	void prevCond();
	void firstCond();
	void trig(bool forced = false);
	void saveAndApplyAll();
	void resetAll();
	void trigOnStatusChanged();
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
