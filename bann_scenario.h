
#ifndef BANN_SCENARIO_H
#define BANN_SCENARIO_H

#include "bann1_button.h" // bannOnSx
#include "bann3_buttons.h" // bann3But
#include "bann4_buttons.h" // bann4But, bann4tasLab
#include "device_status.h"

#include <QObject>
#include <QString>
#include <QList>

class PPTSceDevice;
class scenEvo_cond;
class device;
class QTimerEvent;
class QWidget;

/*!
 * \class bannScenario
 * \brief This class is made to control a scenario of a \a scenario \a unit.
 *
 * \author Davide
 * \date lug 2005
 */
class bannScenario : public bannOnSx
{
Q_OBJECT
public:
	bannScenario(sottoMenu *parent, QString where, QString IconaSx);
private slots:
	void Attiva();
};

/*!
 * \class gesModScen
 * \brief This class is made to control a scenario of a \a din \a scenario \a module.
 *
 * From this banner is possible to actuate, clean and program the scenario controlled.
 * \author Davide
 * \date lug 2005
 */
class gesModScen : public bann4tasLab
{
Q_OBJECT
public:
	gesModScen(QWidget *parent, QString where, QString IcoSx, QString IcoDx, QString IcoCsx,
		QString IcoCdx, QString IcoDes, QString IcoSx2, QString IcoDx2);
public slots:
	void status_changed(QList<device_status*>);
	void inizializza(bool forza = false);
private:
	QString icon_on, icon_stop, icon_info, icon_no_info;
	QString cosa, dove;
	unsigned char sendInProgr, bloccato, in_progr;
	device *dev;
private slots:
	void attivaScenario();
	void enterInfo();
	void exitInfo();
	void startProgScen();
	void stopProgScen();
	void cancScen();
};


/*!
 * \class scenEvo
 * \brief This class represents an advanced scenario management object
 * \author Ciminaghi
 * \date apr 2006
 */
class scenEvo : public bann3But
{
Q_OBJECT
public:
	scenEvo(QWidget *parent, QList<scenEvo_cond*> c, QString i1=QString(), QString i2=QString(), QString i3=QString(), QString i4=QString(), QString act="", int enabled = 0);
	~scenEvo();
	void Draw();
public slots:
	void inizializza(bool forza = false);

private:
	QList<scenEvo_cond*> condList;
	unsigned current_condition;
	QString action;
	int serial_number;
	static int next_serial_number;
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


/*!
 * \class scenSched
 * \brief This class represents a scheduled scenario management object
 * \author Ciminaghi
 * \date apr 2006
 */
class scenSched : public bann4But
{
Q_OBJECT
public:
	scenSched(QWidget *parent, QString Icona1, QString Icona2, QString Icona3, QString Icona4, QString act_enable,
		QString act_disable, QString act_start, QString act_stop);
	/*!
	 * Reimplemented draw
	 */
	void Draw();
public slots:
	/*!
	 * \brief Enable scheduled scenario
	 */
	void enable();
	/*! Disable scheduled scenario */
	void disable();
	/*! Start scheduled scenario */
	void start();
	/*! Stop scheduled scenario */
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
