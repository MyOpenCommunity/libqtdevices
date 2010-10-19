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
	\defgroup Loads Loads

	This section allows the user to manage the loads monitoring system.

	The LoadManagement class presents a list of banners each of which can switch
	to the LoadDataPage and DeactivationTimePage depending on the configuration.

	The classes LoadDataPage and DeactivationTimePage interact with the
	LoadsDevice to work with the system.
*/

/*!
	\ingroup Loads
	\brief Section page that shows a list of BannLoadWithCU and BannLoadNoCU.
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
	\ingroup Loads
	\brief Page that shows a button to confirm a choice.
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
	\ingroup Loads
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
	\ingroup Loads
	\brief Page that interacts with the LoadsDevice class and present the
	LoadDataContent.
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
	\ingroup Loads
	\brief
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
