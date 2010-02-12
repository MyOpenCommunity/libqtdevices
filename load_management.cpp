#include "load_management.h"
#include "xml_functions.h"
#include "bannercontent.h"
#include "main.h"
#include "bann_energy.h"
#include "skinmanager.h" // SkinContext
#include "fontmanager.h" // FontManager
#include "navigation_bar.h" //NavigationBar
#include "bann2_buttons.h" // Bann2Buttons
#include "energy_device.h" // EnergyConversions
#include "loads_device.h" // LoadsDevice
#include "devices_cache.h" // add_device_to_cache

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>


namespace
{

	QString getDescriptionWithPriority(const QDomNode &n)
	{
		QString where = getTextChild(n, "where");
		int pos = where.indexOf('#');
		Q_ASSERT_X(pos > -1, "getDescriptionWithPriority", "Device where must have a '#' character");
		QString descr = getTextChild(n, "descr");
		// remove part after '#'
		QString priority = where.left(pos);
		// remove first 7
		priority = priority.mid(1);
		return priority + ". " + descr;
	}

	bool tryConvert(const QDomNode &n, const QString &node_path, int *value)
	{
		Q_ASSERT_X(value, "tryConvert", "value must be a valid pointer!");
		QDomElement e = getElement(n, node_path);
		bool ok;
		*value = e.text().toInt(&ok);
		if (!ok)
			qDebug() << "tryConvert failed converting node: " << node_path << "Text was: " << e.text();
		return ok;
	}

	bool isRateEnabled(const QDomNode &n)
	{
		int tmp;
		if (tryConvert(n, "rate/ab", &tmp))
			return (tmp == 1);
		else
			return false;
	}

	// Extract rate/rate_id from given node; return INVALID_RATE on errors
	int getRateId(const QDomNode &n)
	{
		int tmp;
		if (tryConvert(n, "rate/rate_id", &tmp))
			return tmp;
		else
			return EnergyRates::INVALID_RATE;
	}

	int getDecimals(const QDomNode &n)
	{
		int tmp;
		if (tryConvert(n, "rate/n_decimal_view", &tmp))
			return tmp;
		else
			return 0;
	}

	// The language used for the floating point number
	QLocale loc(QLocale::Italian);
}
LoadManagement::LoadManagement(const QDomNode &config_node) :
	BannerPage(0)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

void LoadManagement::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		banner *b = getBanner(item);
		if (b)
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
	}
}

banner *LoadManagement::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	LoadsDevice *dev = bt_global::add_device_to_cache(new LoadsDevice(getTextChild(item_node, "where")));
	banner *b = 0;
	switch (id)
	{
	case LOAD_WITH_CU:
	{
		bool advanced = getTextChild(item_node, "advanced").toInt();
		BannLoadWithCU *bann = new BannLoadWithCU(getDescriptionWithPriority(item_node),
			advanced ? BannLoadWithCU::ADVANCED_MODE : BannLoadWithCU::BASE_MODE);
		if (advanced)
		{
			Page *p = new LoadDataPage(item_node, dev);
			bann->connectRightButton(p);
			connect(p, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));
		}
		Page *d = new DeactivationTimePage(item_node, dev);
		connect(bann, SIGNAL(deactivateDevice()), d, SLOT(showPage()));
		connect(d, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));

		b = bann;
	}
		break;
	case LOAD_WITHOUT_CU:
	{
		BannLoadNoCU *bann = new BannLoadNoCU(getTextChild(item_node, "descr"));
		Page *p = new LoadDataPage(item_node, dev);
		bann->connectRightButton(p);
		connect(p, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));
		b = bann;
	}
		break;
	}

	if (b)
		b->setId(id);
	return b;
}


ConfirmationPage::ConfirmationPage(const QString &text)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(cancel()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(accept()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));
	nav_bar->displayScrollButtons(false);

	QLabel *content = new QLabel(text);
	content->setFont(bt_global::font->get(FontManager::SUBTITLE));
	content->setWordWrap(true);

	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}

#define FIRST_PERIOD 0
#define SECOND_PERIOD 1

LoadDataContent::LoadDataContent(int dec, int _rate_id)
{
	current_consumption = new QLabel;
	current_consumption->setText("Current consumption");
	current_consumption->setFont(bt_global::font->get(FontManager::SUBTITLE));
	current_value = 0;

	first_period = new Bann2Buttons;
	first_period->initBanner(QString(), bt_global::skin->getImage("empty_background"), bt_global::skin->getImage("ok"), "data/ora del reset");
	first_period->setCentralText("Total consumption 1");
	first_period_value = 0;
	connect(first_period, SIGNAL(rightClicked()), &mapper, SLOT(map()));
	mapper.setMapping(first_period, FIRST_PERIOD);

	second_period = new Bann2Buttons;
	second_period->initBanner(QString(), bt_global::skin->getImage("empty_background"), bt_global::skin->getImage("ok"), "data/ora del reset");
	second_period->setCentralText("Total consumption 2");
	second_period_value = 0;
	connect(second_period, SIGNAL(rightClicked()), &mapper, SLOT(map()));
	mapper.setMapping(second_period, SECOND_PERIOD);

	connect(&mapper, SIGNAL(mapped(int)), SIGNAL(resetActuator(int)));

	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 0, 0, 0);
	main->setSpacing(0);
	main->addWidget(current_consumption, 0, Qt::AlignHCenter);
	main->addWidget(first_period);
	main->addWidget(second_period);

	rate_id = _rate_id;
	rate = EnergyRates::energy_rates.getRate(rate_id);
	connect(&EnergyRates::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));
	is_currency = false;
	decimals = dec;
}

void LoadDataContent::updatePeriodDate(int period, QDate date, BtTime time)
{
	QString time_str = QString("%1:%2").arg(time.hour()).arg(time.minute(), 2, 10, QChar('0'));
	QString text = date.toString() + " " + time_str;
	if (period == FIRST_PERIOD)
	{
		first_period->setDescriptionText(text);
	}
	else if (period == SECOND_PERIOD)
	{
		second_period->setDescriptionText(text);
	}
	else
		qDebug() << "LoadDataContent::updatePeriodDate: Invalid period";
}

void LoadDataContent::updatePeriodValue(int period, int new_value)
{
	if (period == FIRST_PERIOD)
	{
		first_period_value = new_value;
	}
	else if (period == SECOND_PERIOD)
	{
		second_period_value = new_value;
	}
	else
		qDebug() << "LoadDataContent::updatePeriodValue: Invalid period";
	updateValues();
}

void LoadDataContent::setConsumptionValue(int new_value)
{
	current_value = new_value;
	updateValues();
}

void LoadDataContent::toggleCurrencyView()
{
	is_currency = !is_currency;
	updateValues();
}

void LoadDataContent::rateChanged(int id)
{
	if (id == rate_id)
	{
		rate = EnergyRates::energy_rates.getRate(id);
		updateValues();
	}
}

void LoadDataContent::updateValues()
{
	float current = EnergyConversions::convertToRawData(current_value, EnergyConversions::ELECTRICITY_CURRENT);
	float period1 = EnergyConversions::convertToRawData(first_period_value, EnergyConversions::ELECTRICITY_CURRENT);
	float period2 = EnergyConversions::convertToRawData(second_period_value, EnergyConversions::ELECTRICITY_CURRENT);
	int dec = decimals;
	QString unit_current = "kW";
	QString unit_period = "kWh";
	if (is_currency)
	{
		current = EnergyConversions::convertToMoney(current, rate.rate);
		period1 = EnergyConversions::convertToMoney(period1, rate.rate);
		period2 = EnergyConversions::convertToMoney(period2, rate.rate);
		dec = rate.display_decimals;
		unit_current = unit_period = rate.currency_symbol;
	}

	current_consumption->setText(QString("%1 %2").arg(loc.toString(current, 'f', dec)).arg(unit_current));
	first_period->setCentralText(QString("%1 %2").arg(loc.toString(period1, 'f', dec)).arg(unit_period));
	second_period->setCentralText(QString("%1 %2").arg(loc.toString(period2, 'f', dec)).arg(unit_period));
}


LoadDataPage::LoadDataPage(const QDomNode &config_node, LoadsDevice *d)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	// measure unit here will always be kWh
	QString forward_button;
	int rate_id = -1;
	if (isRateEnabled(config_node))
	{
		forward_button = bt_global::skin->getImage("currency_exchange");
		rate_id = getRateId(config_node);
	}
	int decimals = getDecimals(config_node);
	content = new LoadDataContent(decimals, rate_id);

	NavigationBar *nav_bar = new NavigationBar(forward_button);
	connect(nav_bar, SIGNAL(forwardClick()), content, SLOT(toggleCurrencyView()));

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	ConfirmationPage *confirm = new ConfirmationPage(tr("Are you sure to delete all consumption data?"));
	// show pages correctly
	connect(content, SIGNAL(resetActuator(int)), confirm, SLOT(showPage()));
	connect(confirm, SIGNAL(Closed()), SLOT(showPage()));
	// these connects handle the logic
	connect(content, SIGNAL(resetActuator(int)), SLOT(resetRequested(int)));
	connect(confirm, SIGNAL(accept()), SLOT(reset()));
	reset_number = 0;

	dev = d;
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));

	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 0, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}

void LoadDataPage::resetRequested(int which)
{
	reset_number = which;
}

void LoadDataPage::reset()
{
	dev->resetTotal(reset_number);
}

void LoadDataPage::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	// first, get the period, if any. We need it when we see date and consumption data
	int period = -1;
	if (sl.contains(LoadsDevice::DIM_PERIOD))
		period = sl[LoadsDevice::DIM_PERIOD].toInt();

	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case LoadsDevice::DIM_TOTAL:
			content->updatePeriodValue(period, it.value().toInt());
			break;
		case LoadsDevice::DIM_RESET_DATE:
		{
			QDateTime t = it.value().value<QDateTime>();
			content->updatePeriodDate(period, t.date(), t.time());
		}
			break;
		case LoadsDevice::DIM_CURRENT:
			content->setConsumptionValue(it.value().toInt());
			break;
		}
		++it;
	}
}



DeactivationTimePage::DeactivationTimePage(const QDomNode &config_node, LoadsDevice *d)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	// TODO: cancel user selection?
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(sendDeactivateDevice()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));

	dev = d;

	content = new DeactivationTime(BtTime(2, 30, 0));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 0, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}

void DeactivationTimePage::sendDeactivateDevice()
{
	BtTime t = content->currentTime();
	int off_time = t.hour() * 60 + t.minute();
	dev->forceOff(off_time);
}
