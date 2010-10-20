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

/*!
	\defgroup EnergyData EnergyData
*/


/*!
	\ingroup EnergyData
	\brief Contains the families of energy.

	Each family is represented by a banner, linked with the related EnergyInterface
	page.
*/
class EnergyData : public BannerPage
{
Q_OBJECT
public:
	EnergyData(const QDomNode &config_node);
	virtual int sectionId() const;
	virtual void showPage();

private slots:
	void systemTimeChanged();
	void updateInterfaces();
	void updateDayTimer();

private:
	void loadTypes(const QDomNode &config_node, bool edit_rates);

	QTimer day_timer;
	QList<EnergyInterface *> interfaces;
};


/*!
	\ingroup EnergyData
	\brief Allows the user to set cost and incentive for one or more energy
	typologies.

	The page displays a list of banner, one for each typology or show
	directly the EditEnergyCost page if only one family is defined.
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


/*!
	\ingroup EnergyData
	\brief Sets the cost and incentive for an energy typology.
*/
class EditEnergyCost : public BannerPage
{
Q_OBJECT
public:
	EditEnergyCost();
	void addRate(int rate_id);

	// TODO remove after configurator is fixed (see comment in .cpp)
	virtual void showPage();

private slots:
	void saveRates();
	void resetRates();

private:
	// TODO remove after configurator is fixed
	int production_count, consumption_count;
	bool initialized;
};


/*!
	\ingroup EnergyData
	\brief Contains the interfaces for an energy typology.

	The page shows a BannEnergyInterface banner for each interface or directly
	the related EnergyView page if only one interface is defined.
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

	virtual int sectionId() const;

public slots:
	void toggleCurrency();

private:
	bool loadItems(const QDomNode &config_node);
	bool checkTypeForCurrency(const QString &type, const QDomNode &conf);
	void updateBanners();
	EnergyView *next_page;
	QList<EnergyView*> views;
	static bool is_currency_view;
};

#endif // ENERGY_DATA_H
