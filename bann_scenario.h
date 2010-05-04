/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef BANN_SCENARIO_H
#define BANN_SCENARIO_H

#include "bann1_button.h" // bannOnSx
#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // bann3But
#include "bann4_buttons.h" // bann4But, bann4tasLab
#include "device_status.h"
#include "device.h"        // StatusList

#include <QObject>
#include <QString>
#include <QList>

class PPTSceDevice;
class scenEvo_cond;
class device;
class QTimerEvent;
class QWidget;
class QDomNode;
class ScenarioDevice;



class BannSimpleScenario : public Bann2Buttons
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


// substitute for gesModScen
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
	// if this banner has NOT started scenario programming, it must hide the right (pencil) button when
	// a START_PROG frame arrives
	bool has_started_prog;
};


#if 1
/*!
 * \class bannScenario
 * \brief This class is made to control a scenario of a \a scenario \a unit.
 *
 * \date lug 2005
 */
class bannScenario : public bannOnSx
{
Q_OBJECT
public:
	bannScenario(QWidget *parent, QString where, QString IconaSx);
private slots:
	void Attiva();
};

/*!
 * \class gesModScen
 * \brief This class is made to control a scenario of a \a din \a scenario \a module.
 *
 * From this banner is possible to actuate, clean and program the scenario controlled.
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

#endif


/*!
 * \class scenEvo
 * \brief This class represents an advanced scenario management object
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


// substitute for scenSched
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


#if 0
// DELETE
/*!
 * \class scenSched
 * \brief This class represents a scheduled scenario management object
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
#endif


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
