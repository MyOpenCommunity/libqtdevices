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

#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // Bann3Buttons
#include "bann4_buttons.h" // Bann4ButtonsIcon, bann4But
#include "device.h" // DeviceValues

class PPTSceDevice;
class ScenEvoCondition;
class ScenEvoTimeCondition;
class ScenEvoDeviceCondition;
class QTimerEvent;
class ScenarioDevice;



class BannSimpleScenario : public Bann2Buttons
{
Q_OBJECT
public:
	BannSimpleScenario(int scenario, const QString &descr, const QString &where, int openserver_id);

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
	ScenarioModule(int scenario, const QString &descr, const QString &where, int openserver_id);

private slots:
	void valueReceived(const DeviceValues &values_list);
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
	ScheduledScenario(const QString &enable, const QString &start, const QString &stop, const QString &disable, const QString &descr);

private slots:
	void enable();
	void disable();
	void start();
	void stop();

private:
	QString action_enable, action_disable, action_start, action_stop;
};


class PPTSce : public Bann4Buttons
{
Q_OBJECT
public:
	PPTSce(const QString &descr, const QString &where, int openserver_id);

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
