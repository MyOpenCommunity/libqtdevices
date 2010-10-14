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


#include "bann_energy.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyInterface
#include "btbutton.h"
#include "energy_device.h"
#include "skinmanager.h" // skin
#include "energy_device.h"
#include "loads_device.h"
#include "generic_functions.h" // getBostikName

#include <QLocale>
#include <QDebug>
#include <QGridLayout>

#include <math.h> // pow

// The language used for the floating point number
static QLocale loc(QLocale::Italian);

namespace
{
	QString formatNoSeconds(const BtTime &time)
	{
		QString str = QString("%1:%2").arg(time.hour()).arg(time.minute(), 2, 10, QChar('0'));
		return str;
	}
}

// BannEnergyInterface implementation

BannEnergyInterface::BannEnergyInterface(int rate_id, bool is_ele, const QString &description, EnergyDevice *d) :
	Bann2Buttons(0)
{
	dev = d;

	initBanner(QString(), bt_global::skin->getImage("empty"), bt_global::skin->getImage("select"),
		   description);
	setCentralText("---");

	rate = EnergyRates::energy_rates.getRate(rate_id);
	EnergyRates::energy_rates.setRateDescription(rate_id, description);
	connect(&EnergyRates::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));

	is_electricity = is_ele;
	device_value = INVALID_VALUE;

	connect(dev, SIGNAL(valueReceived(DeviceValues)), this, SLOT(valueReceived(DeviceValues)));
}

void BannEnergyInterface::showEvent(QShowEvent *e)
{
	dev->requestCurrentUpdateStart();
}

void BannEnergyInterface::hideEvent(QHideEvent *e)
{
	dev->requestCurrentUpdateStop();
}

void BannEnergyInterface::setUnitMeasure(const QString &m)
{
	measure = m;
}

void BannEnergyInterface::updateText()
{
	QString text("---");

	if (device_value != INVALID_VALUE)
	{
		float data = EnergyConversions::convertToRawData(device_value,
			is_electricity ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);

		if (EnergyInterface::isCurrencyView() && rate.isValid())
		{
			data = EnergyConversions::convertToMoney(data, rate.rate);
			text = QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(rate.currency_symbol);
		}
		else if (is_electricity)
		{
			if (data >= 1)
				text = QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(measure);
			else
				text = QString("%1 %2").arg(loc.toString(data * 1000, 'f', 0)).arg("W");
		}
		else
			text = QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(measure);
	}

	setCentralText(text);
}

void BannEnergyInterface::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		if (it.key() == EnergyDevice::DIM_CURRENT)
		{
			device_value = it.value().value<EnergyValue>().second;
			updateText();
			break;
		}
		++it;
	}
}

void BannEnergyInterface::rateChanged(int rate_id)
{
	if (rate.id != rate_id)
		return;

	rate = EnergyRates::energy_rates.getRate(rate_id);
	updateText();
}


// BannCurrentEnergy implementation

BannCurrentEnergy::BannCurrentEnergy(const QString &text, EnergyDevice *_dev) :
	Bann2Buttons(0)
{
	initBanner(QString(), bt_global::skin->getImage("bg_banner"), QString(), text);
#ifdef LAYOUT_TS_3_5
	// TODO hack to make the banner align correctly
	static_cast<QGridLayout *>(layout())->setColumnStretch(0, 0);
#endif
	setCentralText("---");
	dev = _dev;
}

void BannCurrentEnergy::showEvent(QShowEvent *e)
{
	dev->requestCurrentUpdateStart();
}

void BannCurrentEnergy::hideEvent(QHideEvent *e)
{
	dev->requestCurrentUpdateStop();
}


// BannEnergyCost implementation

BannEnergyCost::BannEnergyCost(int rate_id, const QString &left, const QString &right,
			       const QString &descr)
{
	rate = EnergyRates::energy_rates.getRate(rate_id);
	current_value = rate.rate;

	// TODO CONFIG_TS_3_5 use rate.description with new config
	initBanner(left, right, " ", FontManager::TEXT, descr, FontManager::TEXT);

	left_button->setAutoRepeat(true);
	right_button->setAutoRepeat(true);

	connect(left_button, SIGNAL(clicked()), SLOT(decRate()));
	connect(right_button, SIGNAL(clicked()), SLOT(incRate()));

	updateLabel();
}

void BannEnergyCost::incRate()
{
	if (current_value + rate.delta < pow(10, rate.display_integers))
	{
		current_value += rate.delta;
		updateLabel();
	}
}

void BannEnergyCost::decRate()
{
	if (current_value - rate.delta >= rate.delta)
	{
		current_value -= rate.delta;
		updateLabel();
	}
}

void BannEnergyCost::updateLabel()
{
	QString unit_measure = rate.currency_symbol + "/" + rate.unit;
	if (rate.mode == 1 || rate.mode == 5)
		unit_measure += "h";

	setCentralText(loc.toString(current_value, 'f', rate.display_decimals) + " " + unit_measure);
}

void BannEnergyCost::saveRate()
{
	rate.rate = current_value;

	EnergyRates::energy_rates.setRate(rate);
}

void BannEnergyCost::resetRate()
{
	current_value = rate.rate;
	updateLabel();
}

int BannEnergyCost::getRateId()
{
	return rate.id;
}


// BannLoadDiagnostic implementation

BannLoadDiagnostic::BannLoadDiagnostic(device *dev, const QString &description) :
	Bann2Buttons(0)
{
	state_icon = bt_global::skin->getImage("state_icon");

	initBanner(state_icon, QString(), description, FontManager::TEXT);
	// the left button is used like a label, to avoid creating yet another banner
	left_button->disable();

	setState(LoadsDevice::LOAD_OK);

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void BannLoadDiagnostic::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(LoadsDevice::DIM_LOAD))
		return;

	setState(values_list[LoadsDevice::DIM_LOAD].toInt());
}

void BannLoadDiagnostic::setState(int state)
{
	left_button->setImage(getBostikName(state_icon, QString::number(state)));
}


// BannLoadNoCU implementation

BannLoadNoCU::BannLoadNoCU(const QString &descr) : Bann3ButtonsLabel(0)
{
	initBanner(QString(), QString(), QString(), bt_global::skin->getImage("load"), bt_global::skin->getImage("info"),
		ENABLED, NOT_FORCED, descr);
}

void BannLoadNoCU::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}


// BannLoadWithCU implementation

BannLoadWithCU::BannLoadWithCU(const QString &descr, LoadsDevice *d, Type t) : Bann3ButtonsLabel(0)
{
	QString right = t == ADVANCED_MODE ? bt_global::skin->getImage("info") : QString();
	initBanner(bt_global::skin->getImage("forced"), bt_global::skin->getImage("not_forced"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("load"), right, ENABLED, NOT_FORCED, descr);
	connect(left_button, SIGNAL(clicked()), SIGNAL(deactivateDevice()));

	dev = d;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	connect(center_button, SIGNAL(clicked()), dev, SLOT(enable()));
}

void BannLoadWithCU::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}

void BannLoadWithCU::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case LoadsDevice::DIM_FORCED:
		{
			bool is_forced = it.value().toBool();
			setForced(is_forced ? FORCED : NOT_FORCED);
			changeLeftFunction(is_forced);
		}
			break;
		case LoadsDevice::DIM_ENABLED:
			setState(it.value().toBool() ? ENABLED : DISABLED);
			// left button is clickable only if the load is disabled
			if (it.value().toBool())
				left_button->enable();
			else
				left_button->disable();
			break;
		}
		++it;
	}
}

void BannLoadWithCU::changeLeftFunction(bool is_forced)
{
	left_button->disconnect();
	is_forced ? connect(left_button, SIGNAL(clicked()), dev, SLOT(forceOn())) :
		connect(left_button, SIGNAL(clicked()), SIGNAL(deactivateDevice()));
}


// It's important that MAX_HOURS is more than max_time below
#define MAX_HOURS 43
// this is 255 values * 10 min = 42h 30min
// This is limit in the protocol; it supports 254 values, each meaning 10' activation time.
static BtTime max_time(42, 20, 0);

DeactivationTime::DeactivationTime(const BtTime &start_time) :
	current_time(start_time)
{
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("plus"), formatNoSeconds(current_time), FontManager::SUBTITLE);
	setTextAlignment(Qt::AlignCenter);
	right_button->setAutoRepeat(true);
	left_button->setAutoRepeat(true);
	connect(right_button, SIGNAL(clicked()), SLOT(plusClicked()));
	connect(left_button, SIGNAL(clicked()), SLOT(minusClicked()));

	max_time.setMaxHours(MAX_HOURS);
	current_time.setMaxHours(MAX_HOURS);
}

BtTime DeactivationTime::currentTime() const
{
	return current_time;
}

void DeactivationTime::setCurrentTime(const BtTime &t)
{
	current_time = t;
}

void DeactivationTime::plusClicked()
{
	// wrap to 10 minutes when at max_time
	if (current_time == max_time)
	{
		current_time = BtTime(0, 10, 0);
		current_time.setMaxHours(MAX_HOURS);
	}
	else
		current_time = current_time.addMinute(10);
	updateDisplay();
}

void DeactivationTime::minusClicked()
{
	// wrap below 10 minutes
	if (current_time.hour() == 0 && current_time.minute() == 10)
		current_time = max_time;
	else
		current_time = current_time.addMinute(-10);
	updateDisplay();
}

void DeactivationTime::updateDisplay()
{
	setCentralText(formatNoSeconds(current_time));
}


