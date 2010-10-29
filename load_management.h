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


#ifndef LOAD_MANAGEMENT_H
#define LOAD_MANAGEMENT_H

#include "page.h"
#include "bannerpage.h"
#include "energy_rates.h" // EnergyRate

#include <QSignalMapper>

class QDomNode;
class banner;
class QLabel;
class DeactivationTime;
class Bann2Buttons;
class LoadsDevice;
class BtTime;
class QDate;

/*!
	\defgroup LoadManagement Load Management

	Allows the user to manage the electrical current loads. It works in two modes:
	- with a central unit;
	- without a central unit.

	In both cases the LoadManagement class shows the list of the loads in the
	system.

	With a central unit, each load (represented by a BannLoadWithCU) has a priority.
	When the central unit detects a system overload, the load with the last
	priority is disabled. The user can force the reactivation of the deactivated
	load, and can also switch a load to manual mode for a
	defined period using the DeactivationTimePage.
	LoadDataPage shows current consumptions and power meters.

	Without central unit the system can only be used to view the current
	consumptions of a load (represented by a BannLoadNoCU) using LoadDataPage as
	in the previous case.
*/


/*!
	\ingroup LoadManagement
	\brief Shows a list of BannLoadWithCU and BannLoadNoCU.
*/
class LoadManagement : public BannerPage
{
Q_OBJECT
public:
	LoadManagement(const QDomNode &config_node);

	virtual int sectionId() const;

	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};


/*!
	\ingroup LoadManagement
	\brief Shows a button to confirm a choice.
*/
class ConfirmationPage : public Page
{
Q_OBJECT
public:
	ConfirmationPage(const QString &text);

signals:
	void accept();
	void cancel();
};


/*!
	\ingroup LoadManagement
	\brief Content for the LoadDataPage that shows data about consuptions.
*/
class LoadDataContent : public QWidget
{
Q_OBJECT
public:
	LoadDataContent(int currency_decimals, int _rate_id = -1);

	void setConsumptionValue(int new_value);

	void updatePeriodDate(int period, const QDate &date, const BtTime &time);
	void updatePeriodValue(int period, qint64 new_value);

public slots:
	void toggleCurrencyView();

private:
	void updateValues();
	QLabel *current_consumption;
	QSignalMapper mapper;
	Bann2Buttons *first_period, *second_period;
	qint64 first_period_value, second_period_value, current_value;
	int rate_id, currency_decimals;
	EnergyRate rate;
	bool is_currency;

private slots:
	void rateChanged(int id);

signals:
	void resetActuator(int);
};


/*!
	\ingroup LoadManagement
	\brief Shows data consumptions and power meters of a load.
*/
class LoadDataPage : public Page
{
Q_OBJECT
public:
	typedef LoadDataContent ContentType;
	LoadDataPage(const QDomNode &config_node, LoadsDevice *d);

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);

private:
	LoadDataContent *content;
	int reset_number;
	LoadsDevice *dev;

private slots:
	void valueReceived(const DeviceValues &values_list);
	void resetRequested(int which);
	void reset();
};


/*!
	\ingroup LoadManagement
	\brief Set the deactivation time of a load.
*/
class DeactivationTimePage : public Page
{
Q_OBJECT
public:
	typedef DeactivationTime ContentType;
	DeactivationTimePage(const QDomNode &config_node, LoadsDevice *d);

private slots:
	void sendDeactivateDevice();

private:
	DeactivationTime *deactivation_time;
	LoadsDevice *dev;
};


#endif // LOAD_MANAGEMENT_H
