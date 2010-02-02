#include "bann_energy.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyInterface

#include <QLocale>

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


bannEnergyInterface::bannEnergyInterface(QWidget *parent, const int rate_id, bool is_ele) :
	bannTextOnImage(parent),
	rate(bt_global::energy_rates.getRate(rate_id))
{
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

