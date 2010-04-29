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


#include "bann_airconditioning.h"
#include "skinmanager.h" // bt_global::skin
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "probe_device.h"
#include "main.h" // (*bt_global::config)
#include "scaleconversion.h" // celsiusString, fahrenheitString, toCelsius, toFahrenheit
#include "generic_functions.h" // setCfgValue()
#include "airconditioning.h" // AdvancedSplitPage
#include "state_button.h"

#include <QLabel> // BannerText
#include <QDebug>
#include <cmath> // round


SingleSplit::SingleSplit(QString descr, bool show_right_button, AirConditioningInterface *d, NonControlledProbeDevice *d_probe) :
	Bann2Buttons(0)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_forward = bt_global::skin->getImage("forward");

	dev_probe = d_probe;
	dev = d;

	QString air_single = "air_single";
	if (dev_probe)
	{
		air_single = "air_single_temp";

		connect(dev_probe, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
		setOpenserverConnection(dev_probe);
	}

	initBanner(img_off, bt_global::skin->getImage(air_single), show_right_button ? img_forward : QString(), descr);
	connect(left_button, SIGNAL(clicked()), SLOT(setDeviceOff()));

	if (dev_probe)
		setCentralText("---");
}

void SingleSplit::valueReceived(const DeviceValues &values_list)
{
	int temp = values_list[NonControlledProbeDevice::DIM_TEMPERATURE].toInt();
	TemperatureScale scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());

	QString text;
	if (scale == FAHRENHEIT)
		text = fahrenheitString(bt2Fahrenheit(temp));
	else
		text = celsiusString(bt2Celsius(temp));
	setCentralText(text);
}

void SingleSplit::setDeviceOff()
{
	dev->turnOff();
}


AdvancedSingleSplit::AdvancedSingleSplit(QString descr, AdvancedSplitPage *p, AirConditioningInterface *d, NonControlledProbeDevice *probe) :
	SingleSplit(descr, true, d, probe)
{
	page = p;
}

void AdvancedSingleSplit::setSerNum(int ser)
{
	banner::setSerNum(ser);
	if (page)
		page->setSerialNumber(ser);
}


GeneralSplit::GeneralSplit(QString descr, bool show_right_button) : Bann2Buttons(0)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_gen = bt_global::skin->getImage("air_general");
	QString img_forward = bt_global::skin->getImage("forward");
	initBanner(img_off, img_air_gen, show_right_button ? img_forward : QString(), descr);
	QObject::connect(left_button, SIGNAL(clicked()), SIGNAL(sendGeneralOff()));
}


AdvancedSplitScenario::AdvancedSplitScenario(const AirConditionerStatus &st, const QString &descr, AdvancedAirConditioningDevice *d, QWidget *parent) :
	Bann2Buttons(parent)
{
	QString icon_cmd = bt_global::skin->getImage("split_cmd");
	initBanner(icon_cmd, QString(), descr);
	dev = d;
	status = st;

	connect(left_button, SIGNAL(clicked()), SLOT(onButtonClicked()));
}

void AdvancedSplitScenario::onButtonClicked()
{
	dev->setStatus(status);
}



CustomScenario::CustomScenario(AdvancedAirConditioningDevice *d) :
	Bann2Buttons(0)
{
	initBanner(QString(), bt_global::skin->getImage("custom_button"), bt_global::skin->getImage("split_settings"), QString());
	dev = d;
}

void CustomScenario::splitValuesChanged(const AirConditionerStatus &st)
{
	dev->setStatus(st);
}




SplitTemperature::SplitTemperature(int init_temp, int level_max, int level_min, int step, int initial_mode) :
	Bann2Buttons(0)
{
	icon_plus = bt_global::skin->getImage("plus");
	icon_minus = bt_global::skin->getImage("minus");
	icon_plus_disabled = bt_global::skin->getImage("plus_disabled");
	icon_minus_disabled = bt_global::skin->getImage("minus_disabled");
	initBanner(icon_minus, icon_plus, "---", FontManager::AIRCONDITIONING_TEMPERATURE);

	scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());
	Q_ASSERT_X(init_temp >= level_min && init_temp <= level_max, "SplitTemperature::SplitTemperature",
		"Initial temperature is outside the given range.");
	current_temp = init_temp;
	max_temp = level_max;
	min_temp = level_min;
	temp_step = step;
	currentModeChanged(initial_mode);

	left_button->setAutoRepeat(true);
	connect(left_button, SIGNAL(clicked()), SLOT(decreaseTemp()));
	right_button->setAutoRepeat(true);
	connect(right_button, SIGNAL(clicked()), SLOT(increaseTemp()));
}

void SplitTemperature::setTemperature(int new_temp_celsius)
{
	int tmp = new_temp_celsius;
	switch (scale)
	{
	case CELSIUS:
		tmp = new_temp_celsius;
		break;
	case FAHRENHEIT:
		tmp = roundTo5(static_cast<int>(toFahrenheit(new_temp_celsius)));
		break;
	}

	if (tmp >= min_temp && tmp <= max_temp)
	{
		current_temp = tmp;
		updateText();
	}
	else
		qWarning() << "SplitTemperature::setTemperature: provided temp is outside limits, ignoring.";
}

void SplitTemperature::currentModeChanged(int new_mode)
{
	switch (new_mode)
	{
	case AdvancedAirConditioningDevice::MODE_DEHUM:
	case AdvancedAirConditioningDevice::MODE_FAN:
		setBannerEnabled(false);
		break;
	default:
		setBannerEnabled(true);
		break;
	}
}

void SplitTemperature::increaseTemp()
{
	int tmp = current_temp + temp_step;
	if (tmp <= max_temp)
	{
		current_temp = tmp;
		updateText();
	}
}

void SplitTemperature::decreaseTemp()
{
	int tmp = current_temp - temp_step;
	if (tmp >= min_temp)
	{
		current_temp = tmp;
		updateText();
	}
}

void SplitTemperature::updateText()
{
	if (is_enabled)
	{
		switch (scale)
		{
		case CELSIUS:
			setCentralText(celsiusString(current_temp));
			break;
		case FAHRENHEIT:
			setCentralText(fahrenheitString(current_temp));
			break;
		}
	}
	else
	{
		// in this case the text must be "--.- C/F" (depending on scale)
		QString text("--.- " TEMP_DEGREES);
		switch (scale)
		{
		case CELSIUS:
			text += "C";
			break;
		case FAHRENHEIT:
			text += "F";
			break;
		}

		setCentralText(text);
	}
}

int SplitTemperature::temperature()
{
	switch (scale)
	{
	case FAHRENHEIT:
		return static_cast<int>(round(toCelsius(current_temp)));
	case CELSIUS:
	default:
		return current_temp;
	}
}

int SplitTemperature::roundTo5(int temp)
{
	// 13/5 = 2 ---> (2 + 1)*5 = 15
	// 12%5 = 2 ---> 12 - 2 = 10;
	int div = temp / 5;
	int mod = temp % 5;
	if (mod == 0)
		return temp;
	else
	{
		if (mod < 3)
			return temp - mod;
		else
			return (div + 1) * 5;
	}
}

void SplitTemperature::setBannerEnabled(bool enable)
{
	is_enabled = enable;
	if (is_enabled)
	{
		left_button->enable();
		left_button->setImage(icon_minus);
		right_button->enable();
		right_button->setImage(icon_plus);
	}
	else
	{
		left_button->disable();
		left_button->setImage(icon_minus_disabled);
		right_button->disable();
		right_button->setImage(icon_plus_disabled);
	}
	updateText();
}


SplitMode::SplitMode(QList<int> modes, int current_mode) : BannStates(0)
{
	modes_descr[AdvancedAirConditioningDevice::MODE_OFF] = tr("OFF");
	modes_descr[AdvancedAirConditioningDevice::MODE_WINTER] = tr("HEATING");
	modes_descr[AdvancedAirConditioningDevice::MODE_SUMMER] = tr("COOLING");
	modes_descr[AdvancedAirConditioningDevice::MODE_DEHUM] = tr("DRY");
	modes_descr[AdvancedAirConditioningDevice::MODE_FAN] = tr("FAN");
	modes_descr[AdvancedAirConditioningDevice::MODE_AUTO] = tr("AUTO");

	foreach (int mode_id, modes)
		if (modes_descr.contains(mode_id))
			addState(mode_id, modes_descr[mode_id]);
		else
			qWarning("The mode id %d doesn't exists", mode_id);

	initBanner(bt_global::skin->getImage("cycle_mode"), current_mode);
	connect(this, SIGNAL(stateChanged(int)), SIGNAL(modeChanged(int)));
}



SplitSpeed::SplitSpeed(QList<int> speeds, int current_speed) : BannStates(0)
{
	speeds_descr[AdvancedAirConditioningDevice::VEL_AUTO] = tr("AUTO");
	speeds_descr[AdvancedAirConditioningDevice::VEL_MIN] = tr("LOW");
	speeds_descr[AdvancedAirConditioningDevice::VEL_MED] = tr("MEDIUM");
	speeds_descr[AdvancedAirConditioningDevice::VEL_MAX] = tr("HIGH");
	speeds_descr[AdvancedAirConditioningDevice::VEL_SILENT] = tr("SILENT");

	foreach (int speed_id, speeds)
		if (speeds_descr.contains(speed_id))
			addState(speed_id, speeds_descr[speed_id]);
		else
			qWarning("The speed id %d doesn't exists", speed_id);

	initBanner(bt_global::skin->getImage("cycle_speed"), current_speed);
}


// The button group guarantees that only one button is pressed at a given time.
// Button ids are chosen so that they can be converted to bool values, ie. are 0 or 1.
SplitSwing::SplitSwing(QString descr, bool init_swing) : Bann2StateButtons(0)
{
	initBanner(bt_global::skin->getImage("swing_off"), bt_global::skin->getImage("swing_on"), descr);

	// left = OFF button, so id is 0
	buttons.addButton(left_button, 0);
	// right = ON button, so id is 1
	buttons.addButton(right_button, 1);
	buttons.setExclusive(true);

	right_button->setOffImage(bt_global::skin->getImage("swing_on"));
	right_button->setOnImage(bt_global::skin->getImage("swing_on_checked"));
	left_button->setOnImage(bt_global::skin->getImage("swing_off_checked"));
	left_button->setOffImage(bt_global::skin->getImage("swing_off"));

	right_button->setCheckable(true);
	left_button->setCheckable(true);
	setSwingOn(init_swing);
}

void SplitSwing::setSwingOn(bool swing_on)
{
	swing_on ? right_button->setChecked(true) : left_button->setChecked(true);
}

bool SplitSwing::swing()
{
	return buttons.checkedId();
}


SplitScenario::SplitScenario(QString descr, QString cmd, AirConditioningDevice *d) : Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("split_cmd"), QString(), descr);
	command = cmd;
	dev = d;
	connect(left_button, SIGNAL(clicked()), SLOT(sendScenarioCommand()));
}

void SplitScenario::sendScenarioCommand()
{
	dev->sendCommand(command);
}


GeneralSplitScenario::GeneralSplitScenario(QString descr) : Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("split_cmd"), QString(), descr);
	connect(left_button, SIGNAL(clicked()), SLOT(sendScenarioCommand()));
}

void GeneralSplitScenario::appendDevice(QString cmd, AirConditioningInterface *d)
{
	devices_list.append(qMakePair(cmd, d));
}

void GeneralSplitScenario::sendScenarioCommand()
{
	for (int i = 0; i < devices_list.size(); ++i)
		devices_list[i].second->activateScenario(devices_list[i].first);
}
