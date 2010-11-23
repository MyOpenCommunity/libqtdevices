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


#ifndef BANN_ENERGY_H
#define BANN_ENERGY_H

#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // Bann3ButtonsLabel
#include "energy_rates.h"  // EnergyRate
#include "device.h"        // DeviceValues
#include "bttime.h" // BtTime

struct EnergyRate;
class EnergyDevice;
class LoadsDevice;


/*!
	\ingroup EnergyData
	\brief Represents a single interface.
*/
class BannEnergyInterface : public Bann2Buttons
{
Q_OBJECT
public:
	BannEnergyInterface(int rate_id, bool is_ele, const QString &description, EnergyDevice *d);
	void setUnitMeasure(const QString &m);
	void updateText();

public slots:
	void valueReceived(const DeviceValues &values_list);

protected:
	virtual void showEvent(QShowEvent *e);
	virtual void hideEvent(QHideEvent *e);

private slots:
	void rateChanged(int rate_id);

private:
	static const unsigned int INVALID_VALUE = (unsigned int)-1;

	unsigned int device_value;
	QString measure;
	bool is_electricity;
	EnergyRate rate;
	EnergyDevice *dev;
};


/*!
	\ingroup EnergyData
	\brief Displays the current consumptions for an interface.
*/
class BannCurrentEnergy : public Bann2Buttons
{
public:
	BannCurrentEnergy(const QString &text, EnergyDevice *dev);

protected:
	virtual void showEvent(QShowEvent *e);
	virtual void hideEvent(QHideEvent *e);

private:
	EnergyDevice *dev;
};


/*!
	\ingroup EnergyData
	\brief Allows modifying the tariff for a given family of energy.
*/
class BannEnergyCost : public Bann2Buttons
{
Q_OBJECT
public:
	BannEnergyCost(int rate_id, const QString &left, const QString &right,
		       const QString &descr);

	void saveRate();
	void resetRate();

	int getRateId();

private:
	void updateLabel();

private slots:
	void incRate();
	void decRate();

private:
	EnergyRate rate;
	float current_value;
};


/*!
	\ingroup LoadDiagnostic
	\brief Shows the status of a load (normal, close to limit, above limit)
*/
class BannLoadDiagnostic : public Bann2Buttons
{
Q_OBJECT
public:
	BannLoadDiagnostic(device *dev, const QString &description);

private:
	void setState(int state);

private slots:
	void valueReceived(const DeviceValues &values_list);

private:
	QString state_icon;
};


/*!
	\ingroup LoadManagement
	\brief Represents a load when the \ref LoadManagement has not a central unit.
*/
class BannLoadNoCU : public Bann3ButtonsLabel
{
Q_OBJECT
public:
	BannLoadNoCU(const QString &descr);
	void connectRightButton(Page *p);
};


/*!
	\ingroup LoadManagement
	\brief Represents a load when the \ref LoadManagement has a central unit.
*/
class BannLoadWithCU : public Bann3ButtonsLabel
{
Q_OBJECT
public:
	enum Type
	{
		BASE_MODE,
		ADVANCED_MODE,
	};
	BannLoadWithCU(const QString &descr, LoadsDevice *d, Type t);
	void connectRightButton(Page *p);

private:
	void changeLeftFunction(bool is_forced);
	LoadsDevice *dev;

private slots:
	void valueReceived(const DeviceValues &values_list);

signals:
	// this should be emitted only if the device is active
	void deactivateDevice();
};


/*!
	\ingroup LoadManagement
	\brief Sets the time of deactivation for a load.
*/
class DeactivationTime : public Bann2Buttons
{
Q_OBJECT
public:
	DeactivationTime(const BtTime &start_time);
	// TODO: this interface supports user cancel. If it's not needed, just use a timeChanged() signal
	BtTime currentTime() const;
	void setCurrentTime(const BtTime &t);

private:
	void updateDisplay();
	BtTime current_time;

private slots:
	void plusClicked();
	void minusClicked();
};

#endif // BANN_ENERGY_H
