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


#ifndef SCENEVODEVICESCONDDISPLAY_H
#define SCENEVODEVICESCONDDISPLAY_H

#include "scenevodevicescond.h"

#include <QWidget>


/*!
	\ingroup Scenarios
	\brief Displays a device condition.
	\sa DeviceConditionDisplayInterface
*/
class DeviceConditionDisplay : public QWidget, public DeviceConditionDisplayInterface
{
Q_OBJECT
public:
	DeviceConditionDisplay(QWidget *parent, QString descr, QString top_icon);

signals:
	void upClicked();
	void downClicked();

protected:
	QLabel *condition;
	BtButton *up_button, *down_button;
};


/*!
	\ingroup Scenarios
	\brief Displays a device condition which can have only the ON or the OFF status.
*/
class DeviceConditionDisplayOnOff : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayOnOff(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/*!
	\ingroup Scenarios
	\brief Displays a dimming device condition.
*/
class DeviceConditionDisplayDimming : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayDimming(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/*!
	\ingroup Scenarios
	\brief Displays a dimming 100 device condition.
*/
class DeviceConditionDisplayDimming100 : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayDimming100(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/*!
	\ingroup Scenarios
	\brief Displays a volume device condition.
*/
class DeviceConditionDisplayVolume : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayVolume(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


 /*!
	\ingroup Scenarios
	\brief Displays a temperature device condition (Celsius or Fahrenheit,
	depending on the configuration).
*/
class DeviceConditionDisplayTemperature : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayTemperature(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};



#endif // SCENEVODEVICESCONDDISPLAY_H

