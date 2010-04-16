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

class LoadManagement : public BannerPage
{
Q_OBJECT
public:
	LoadManagement(const QDomNode &config_node);

	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};


class ConfirmationPage : public Page
{
Q_OBJECT
public:
	ConfirmationPage(const QString &text);

signals:
	void accept();
	void cancel();
};


class LoadDataContent : public QWidget
{
Q_OBJECT
public:
	/**
	 * \param dec Number of decimals to display when in energy view
	 * \param _rate_id Rate id to be used for economic data conversions
	 */
	LoadDataContent(int dec, int _rate_id = -1);

	/**
	 * Set text on consumption label
	 */
	void setConsumptionValue(int new_value);

	/**
	 * Update time frame for the given period.
	 *
	 * \param period Extracted from frame
	 */
	void updatePeriodDate(int period, const QDate &date, const BtTime &time);

	/**
	 * Update consuption value for the given period.
	 *
	 * \param period Extracted from frame
	 */
	void updatePeriodValue(int period, int new_value);

public slots:
	void toggleCurrencyView();

private:
	void updateValues();
	QLabel *current_consumption;
	QSignalMapper mapper;
	Bann2Buttons *first_period, *second_period;
	int first_period_value, second_period_value, current_value;
	int rate_id, decimals;
	EnergyRate rate;
	bool is_currency;

private slots:
	void rateChanged(int id);

signals:
	void resetActuator(int);
};

class LoadDataPage : public Page
{
Q_OBJECT
public:
	typedef LoadDataContent ContentType;
	// TODO: add LoadManagement device
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



class DeactivationTimePage : public Page
{
Q_OBJECT
public:
	typedef DeactivationTime ContentType;
	DeactivationTimePage(const QDomNode &config_node, LoadsDevice *d);

private slots:
	void sendDeactivateDevice();

private:
	DeactivationTime *content;
	LoadsDevice *dev;
};

#endif // LOAD_MANAGEMENT_H
