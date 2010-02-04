#include "energy_rates.h"
#include "xml_functions.h"
#include "main.h" // getConfElement
#include "generic_functions.h" // setGlobalCfgValue

#include <QLocale>


static QLocale loc(QLocale::Italian);
static EnergyRate invalid_rate;

EnergyRates bt_global::energy_rates;


bool EnergyRate::isValid() const
{
	return !currency_name.isEmpty();
}


EnergyRates::EnergyRates()
{
}

void EnergyRates::loadRates()
{
	if (!rates.isEmpty())
		return;

	const QDomNode &conf_node = getConfElement("displaypages/rate_info");

	foreach (const QDomNode &item, getChildren(conf_node, "item"))
	{
		EnergyRate rate;

		rate.id = getTextChild(item, "rate_id").toInt();
		rate.description = getTextChild(item, "descr");
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
	if (rate_id == -1)
		return invalid_rate;

	Q_ASSERT_X(rates.contains(rate_id), "EnergyRates::rate", "missing rate");

	return rates[rate_id];
}

QList<int> EnergyRates::allRateId() const
{
	return rates.keys();
}

void EnergyRates::setRate(const EnergyRate &new_rate)
{
	Q_ASSERT_X(rates.contains(new_rate.id), "EnergyRates::setRate", "missing rate");

	rates[new_rate.id] = new_rate;
	emit rateChanged(new_rate.id);

	// CONFIG_BTOUCH implement for touchx
	QMap<QString, QString> map;

	map["tariff"] = loc.toString(new_rate.rate, 'f', 3);
#ifdef CONFIG_BTOUCH
	// TODO needs a test TouchX config
	setGlobalCfgValue(map, "rate_id", new_rate.id);
#endif
}
