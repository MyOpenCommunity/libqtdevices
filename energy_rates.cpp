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


#include "energy_rates.h"
#include "xml_functions.h"
#include "main.h" // getConfElement
#include "generic_functions.h" // setGlobalCfgValue

#include <QLocale>
#include <QDomNode>
#include <QMap>


static QLocale loc(QLocale::Italian);
static EnergyRate invalid_rate;

EnergyRates EnergyRates::energy_rates;


bool EnergyRate::isValid() const
{
	return !currency_name.isEmpty();
}


EnergyRates::EnergyRates()
{
}

bool EnergyRates::hasRates()
{
	return !rates.isEmpty();
}

void EnergyRates::loadRates()
{
	if (!rates.isEmpty())
		return;

#ifdef CONFIG_BTOUCH
	const QDomNode &conf_node = getConfElement("displaypages/rate_info");
#else
	const QDomNode &conf_node = getConfElement("tariff_setting/rate_info");
#endif
	foreach (const QDomNode &item, getChildren(conf_node, "item"))
	{
		EnergyRate rate;

		rate.id = getTextChild(item, "rate_id").toInt();
		// TODO use this after the configurator is fixed (see comment in energy_data.cpp)
		// rate.description = getTextChild(item, "descr");
		rate.description = " ";
		rate.currency_name = getTextChild(item, "name");
		rate.mode = getTextChild(item, "mode").toInt();
		rate.is_production = getTextChild(item, "type").toInt() == 1;
		rate.unit = getTextChild(item, "measure");
		rate.currency_symbol = getTextChild(item, "symbol");
		rate.display_integers = getTextChild(item, "n_integer").toInt();
		rate.display_decimals = getTextChild(item, "n_decimal").toInt();

		bool ok;

		rate.rate = loc.toFloat(getTextChild(item, "tariff"), &ok);
		if (!ok)
			qFatal("Rate is in wrong format, you should use ',' instead of '.'");

		rate.delta = loc.toFloat(getTextChild(item, "delta"), &ok);
		if (!ok)
			qFatal("Delta is in wrong format, you should use ',' instead of '.'");

		rates[rate.id] = rate;
	}
}

EnergyRate EnergyRates::getRate(int rate_id) const
{
	if (rate_id == INVALID_RATE)
		return invalid_rate;

	Q_ASSERT_X(rates.contains(rate_id), "EnergyRates::rate", qPrintable(QString("missing rate %1").arg(rate_id)));

	return rates[rate_id];
}

QList<int> EnergyRates::allRateId() const
{
	return rates.keys();
}

void EnergyRates::setRateDescription(int rate_id, const QString &description)
{
	if (rate_id == INVALID_RATE)
		return;

	// if only one interface uses this rate, use the interface name, otherwise
	// the rate name is generated by EditEnergyCost::showPage()
	if (rates[rate_id].description.isEmpty())
		rates[rate_id].description = description;
	else
		rates[rate_id].description = " ";
}

void EnergyRates::setRate(const EnergyRate &new_rate)
{
	Q_ASSERT_X(rates.contains(new_rate.id), "EnergyRates::setRate", "missing rate");

	rates[new_rate.id] = new_rate;
	emit rateChanged(new_rate.id);

	QMap<QString, QString> map;

	map["tariff"] = loc.toString(new_rate.rate, 'f', 3);

#ifdef CONFIG_BTOUCH
	QString root_name = "displaypages";
#else
	QString root_name = "tariff_setting";
#endif
	setGlobalCfgValue(root_name, map, "rate_id", new_rate.id);
}
