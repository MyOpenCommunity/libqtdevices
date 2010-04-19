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


#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include "main.h"
#include "datetime.h" //BtTimeEdit

#include <QTimer>


class BtButton;
class DeviceCondition;

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


#endif // _SCENEVOCOND_H_
