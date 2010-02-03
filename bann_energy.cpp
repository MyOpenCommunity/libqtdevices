#include "bann_energy.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyInterface
#include "btbutton.h"

#include <QLocale>

#include <math.h> // pow

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


// bannEnergyInterface implementation

bannEnergyInterface::bannEnergyInterface(QWidget *parent, int rate_id, bool is_ele) :
	bannTextOnImage(parent)
{
	rate = bt_global::energy_rates.getRate(rate_id);
	connect(&bt_global::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));

	is_electricity = is_ele;
	device_value = 0;
}

void bannEnergyInterface::setUnitMeasure(const QString &m)
{
	measure = m;
}

void bannEnergyInterface::updateText()
{
	QString text("---");

	if (device_value)
	{
		float data = EnergyConversions::convertToRawData(device_value,
			is_electricity ? EnergyConversions::ELECTRICITY_CURRENT : EnergyConversions::DEFAULT_ENERGY);

		if (EnergyInterface::isCurrencyView() && rate.isValid())
		{
			data = EnergyConversions::convertToMoney(data, rate.rate);
			text = QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(rate.currency_symbol);
		}
		else
			text = QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(measure);
	}

	setInternalText(text);
}

void bannEnergyInterface::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == EnergyDevice::DIM_CURRENT)
		{
			device_value = it.value().value<EnergyValue>().second;
			updateText();
			// TODO: is this necessary?
			Draw();
			break;
		}
		++it;
	}
}

void bannEnergyInterface::rateChanged(int rate_id)
{
	if (rate.id != rate_id)
		return;

	rate = bt_global::energy_rates.getRate(rate_id);
	updateText();
}


// BannEnergyCost implementation

BannEnergyCost::BannEnergyCost(int rate_id, const QString &left, const QString &right,
			       const QString &descr)
{
	rate = bt_global::energy_rates.getRate(rate_id);
	current_value = rate.rate;

	// TODO BTOUCH_CONFIG use rate.description with new config
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
