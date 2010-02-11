#include "bann_energy.h"
#include "energy_rates.h"
#include "energy_data.h" // EnergyInterface
#include "btbutton.h"
#include "energy_device.h"
#include "skinmanager.h" // skin
#include "energy_device.h"
#include "loads_device.h"
#include "generic_functions.h" // getBostikName

#include <QLocale>
#include <QDebug>
#include <QGridLayout>

#include <math.h> // pow

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


// BannEnergyInterface implementation

BannEnergyInterface::BannEnergyInterface(int rate_id, bool is_ele, const QString &description) :
	Bann2Buttons(0)
{
	initBanner(QString(), bt_global::skin->getImage("empty"), bt_global::skin->getImage("select"),
		   description);
	setCentralText("---");

	rate = EnergyRates::energy_rates.getRate(rate_id);
	connect(&EnergyRates::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));

	is_electricity = is_ele;
	device_value = 0;
}

void BannEnergyInterface::setUnitMeasure(const QString &m)
{
	measure = m;
}

void BannEnergyInterface::updateText()
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

	setCentralText(text);
}

void BannEnergyInterface::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == EnergyDevice::DIM_CURRENT)
		{
			device_value = it.value().value<EnergyValue>().second;
			updateText();
			break;
		}
		++it;
	}
}

void BannEnergyInterface::rateChanged(int rate_id)
{
	if (rate.id != rate_id)
		return;

	rate = EnergyRates::energy_rates.getRate(rate_id);
	updateText();
}


// BannCurrentEnergy implementation

BannCurrentEnergy::BannCurrentEnergy(const QString &text, EnergyDevice *_dev) :
	Bann2Buttons(0)
{
	initBanner(QString(), bt_global::skin->getImage("bg_banner"), QString(), text);
	// TODO hack to make the banner align correctly
	static_cast<QGridLayout *>(layout())->setColumnStretch(0, 0);
	setCentralText("---");
	dev = _dev;
}

void BannCurrentEnergy::showEvent(QShowEvent *e)
{
	dev->requestCurrentUpdateStart();
}

void BannCurrentEnergy::hideEvent(QHideEvent *e)
{
	dev->requestCurrentUpdateStop();
}


// BannEnergyCost implementation

BannEnergyCost::BannEnergyCost(int rate_id, const QString &left, const QString &right,
			       const QString &descr)
{
	rate = EnergyRates::energy_rates.getRate(rate_id);
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

void BannEnergyCost::saveRate()
{
	rate.rate = current_value;

	EnergyRates::energy_rates.setRate(rate);
}

void BannEnergyCost::resetRate()
{
	current_value = rate.rate;
	updateLabel();
}


// BannLoadDiagnostic implementation

BannLoadDiagnostic::BannLoadDiagnostic(device *dev, const QString &description) :
	Bann2Buttons(0)
{
	state_icon = bt_global::skin->getImage("state_icon");

	initBanner(state_icon, QString(), description, FontManager::TEXT);
	// the left button is used like a label, to avoid creating yet another banner
	left_button->disable();

	setState(LoadsDevice::LOAD_OK);

	connect(dev, SIGNAL(status_changed(StatusList)), SLOT(status_changed(StatusList)));
}

void BannLoadDiagnostic::status_changed(const StatusList &sl)
{
	if (!sl.contains(LoadsDevice::DIM_LOAD))
		return;

	setState(sl[LoadsDevice::DIM_LOAD].toInt());
}

void BannLoadDiagnostic::setState(int state)
{
	left_button->setImage(getBostikName(state_icon, QString::number(state)));
}


// BannLoadNoCU implementation

BannLoadNoCU::BannLoadNoCU(const QString &descr) : Bann3ButtonsLabel(0)
{
	initBanner(QString(), QString(), QString(), bt_global::skin->getImage("load"), bt_global::skin->getImage("info"),
		ENABLED, NOT_FORCED, descr);
}

void BannLoadNoCU::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}


// BannLoadWithCU implementation

BannLoadWithCU::BannLoadWithCU(const QString &descr, Type t) : Bann3ButtonsLabel(0)
{
	QString right = t == ADVANCED_MODE ? bt_global::skin->getImage("info") : QString();
	initBanner(bt_global::skin->getImage("forced"), bt_global::skin->getImage("not_forced"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("load"), right, ENABLED, NOT_FORCED, descr);
	// TODO: this should be changed on mode change too
	connect(left_button, SIGNAL(clicked()), SIGNAL(deactivateDevice()));
}

void BannLoadWithCU::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}


DeactivationTime::DeactivationTime(const BtTime &start_time) :
	current_time(start_time)
{
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("plus"), formatTime(current_time), FontManager::SUBTITLE);
	right_button->setAutoRepeat(true);
	left_button->setAutoRepeat(true);
	connect(right_button, SIGNAL(clicked()), SLOT(plusClicked()));
	connect(left_button, SIGNAL(clicked()), SLOT(minusClicked()));
}

BtTime DeactivationTime::currentTime() const
{
	return current_time;
}

void DeactivationTime::setCurrentTime(const BtTime &t)
{
	current_time = t;
}

void DeactivationTime::plusClicked()
{
	current_time = current_time.addMinute(1);
	updateDisplay();
}

void DeactivationTime::minusClicked()
{
	current_time = current_time.addMinute(-1);
	updateDisplay();
}

void DeactivationTime::updateDisplay()
{
	setCentralText(formatTime(current_time));
}


