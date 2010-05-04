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


#ifndef ENERGY_DATA_H
#define ENERGY_DATA_H

#include "bannerpage.h"

#include <QTimer>

class banner;
class QDomNode;
class EnergyView;
class EnergyInterface;
struct EnergyRate;


/**
 * The page that contains the families of energy.
 */
class EnergyData : public BannerPage
{
Q_OBJECT
public:
	EnergyData(const QDomNode &config_node);

	virtual void showPage();
	virtual int sectionId() const;

private slots:
	void systemTimeChanged();
	void updateInterfaces();
	void updateDayTimer();

private:
	void loadTypes(const QDomNode &config_node, bool edit_rates);

	QTimer day_timer;
	QList<EnergyInterface *> interfaces;
};


/**
 * The page to set the related cost and incentive of an energy typology.
 */
class EnergyCost : public BannerPage
{
Q_OBJECT
public:
	EnergyCost();

	virtual void showPage();

private:
	Page *next_page;
};


class EditEnergyCost : public BannerPage
{
Q_OBJECT
public:
	EditEnergyCost();

	void addRate(int rate_id);

signals:
	void prodValueChanged(float);
	void consValueChanged(float);

private slots:
	void saveRates();
	void resetRates();

private:
	// TODO CONFIG_BTOUCH
	int production_count, consumption_count;
};


/**
 * A page container of interfaces banners.
 */
class EnergyInterface : public BannerPage
{
Q_OBJECT
public:
	EnergyInterface(const QDomNode &config_node, bool edit_rates, bool parent_skipped);

	virtual void showPage();
	static void toggleCurrencyView();
	static bool isCurrencyView();
	void systemTimeChanged();

public slots:
	void toggleCurrency();

private:
	void loadItems(const QDomNode &config_node, NavigationBar *nav_bar);
	bool checkTypeForCurrency(const QString &type, const QDomNode &conf);
	void updateBanners();
	EnergyView *next_page;
	QList<EnergyView*> views;
	bool is_any_interface_enabled;
	static bool is_currency_view;
};

#endif // ENERGY_DATA_H
