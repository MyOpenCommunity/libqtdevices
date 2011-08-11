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

#include <QTimer>
#include <QWidget>
#include <QTime>

class BtTimeEdit;
class BtButton;
class DeviceCondition;

/*!
	\ingroup Scenarios
	\brief An abstract page that represents an improved scenario condition.
*/
class ScenEvoCondition : public QWidget
{
Q_OBJECT
public:
	void set_serial_number(int);
	int get_serial_number();

public slots:
	/*!
		\brief Applies changes to the condition.
	*/
	virtual void Apply() = 0;

	/*!
		\brief Saves changes to the condition in the configuration file.
	*/
	virtual void save() = 0;

	/*!
		\brief Resets the condition, discarding changes.
	*/
	virtual void reset() = 0;

signals:
	/*!
		\brief Emitted when the related condition is satisfied.
	*/
	void condSatisfied();

protected:
	int item_id;

private:
	int serial_number;
};


/*!
	\ingroup Scenarios
	\brief Represents a time based evolved scenario condition.
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
	void scaduta();
	void setupTimer();

private:
	QTime cond_time;
	BtButton *bottom_left, *bottom_center, *bottom_right;

	BtTimeEdit *time_edit;
	QTimer timer;
};


/*!
	\ingroup Scenarios
	\brief Represents a device based evolved scenario condition.
*/
class ScenEvoDeviceCondition : public ScenEvoCondition
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Build the object, creating also the proper DeviceConditionDisplayInterface
		and DeviceCondition objects, depending on the configuration.
	*/
	ScenEvoDeviceCondition(int item_id, const QDomNode &config_node);
	~ScenEvoDeviceCondition();

	/*!
		\brief Returns true when the condition is satisfied.
	*/
	virtual bool isTrue();

	QString getDescription() { return description; }
	QString getIcon() { return icon; }

public slots:
	virtual void save();
	virtual void Apply();
	virtual void reset();

private:
	// Specific device condition
	DeviceCondition *device_cond;

	QString description, icon;
};


#endif // _SCENEVOCOND_H_
