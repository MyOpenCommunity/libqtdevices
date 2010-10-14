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


#include "load_management.h"
#include "xml_functions.h"
#include "main.h"
#include "bann_energy.h"
#include "skinmanager.h" // SkinContext
#include "fontmanager.h" // FontManager
#include "navigation_bar.h" //NavigationBar
#include "bann2_buttons.h" // Bann2Buttons
#include "energy_device.h" // EnergyConversions
#include "loads_device.h" // LoadsDevice
#include "devices_cache.h" // add_device_to_cache
#include "bttime.h" // BtTime
#include "generic_functions.h" // DateConversion::formatDateConfig
#include "energy_management.h" // isRateEditDisplayed, isBuilt
#include "energy_data.h" // EnergyCost
#include "btbutton.h"

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QDate>


enum
{
#ifdef CONFIG_TS_3_5
	LOAD_WITH_CU=80,
	LOAD_WITHOUT_CU=81,
#else
	LOAD_WITH_CU=18001,
	LOAD_WITHOUT_CU=18002,
#endif
};


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

	QWidget *buildTitle(const QString &title)
	{
		QLabel *page_title = new QLabel(title);
		page_title->setAlignment(Qt::AlignCenter);
		page_title->setFont(bt_global::font->get(FontManager::TEXT));
		return page_title;
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
#ifdef CONFIG_TS_3_5
		if (tryConvert(n, "rate/ab", &tmp))
#else
		if (tryConvert(n, "rate/enabled", &tmp))
#endif
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
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	EnergyRates::energy_rates.loadRates();
#ifdef LAYOUT_TS_10
	QString title = getTextChild(config_node, "descr");
#endif

	// display the button to edit rates if more than one family
	if (EnergyRates::energy_rates.hasRates() && !EnergyManagement::isRateEditDisplayed())
	{
		Page *costs = new EnergyCost;
		connect(costs, SIGNAL(Closed()), SLOT(showPage()));

#ifdef LAYOUT_TS_3_5
		NavigationBar *nav = new NavigationBar(bt_global::skin->getImage("currency_exchange"));
		buildPage(new BannerContent, nav);
		connect(this, SIGNAL(forwardClick()), costs, SLOT(showPage()));
#else
		QWidget *content = new QWidget;

		QVBoxLayout *vlayout = new QVBoxLayout;
		BannerContent *banner_content = new BannerContent;
		vlayout->addWidget(banner_content, 2);

		QHBoxLayout *buttons_layout = new QHBoxLayout;
		buttons_layout->addStretch(2);

		BtButton *costs_button = new BtButton(bt_global::skin->getImage("currency_exchange"));
		connect(costs_button, SIGNAL(clicked()), costs, SLOT(showPage()));

		buttons_layout->addWidget(costs_button, 1, Qt::AlignCenter);

		vlayout->addLayout(buttons_layout, 1);

		content->setLayout(vlayout);
		buildPage(content, banner_content, new NavigationBar, title, TITLE_HEIGHT);
#endif

	}
	else
	{
#ifdef LAYOUT_TS_3_5
		buildPage();
#else
		buildPage(title, TITLE_HEIGHT);
#endif
	}
	loadItems(config_node);
}

int LoadManagement::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : LOAD_MANAGEMENT;
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
		BannLoadWithCU *bann = new BannLoadWithCU(getDescriptionWithPriority(item_node), dev,
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
	QWidget *content = NULL;
	QLabel *label = new QLabel(text);

#ifdef LAYOUT_TS_3_5
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));

	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(accept()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));

	content = label;
#else
	NavigationBar *nav_bar = new NavigationBar;
	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));

	connect(ok, SIGNAL(clicked()), SIGNAL(accept()));
	connect(ok, SIGNAL(clicked()), SIGNAL(Closed()));

	content = new QWidget;
	QGridLayout *l = new QGridLayout(content);
	l->setContentsMargins(5, 5, 25, 47);
	l->setColumnStretch(0, 1);

	l->addWidget(label, 0, 0);
	l->addWidget(ok, 1, 1);
#endif

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(cancel()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	nav_bar->displayScrollButtons(false);

	label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	label->setWordWrap(true);
	label->setAlignment(Qt::AlignHCenter);
	label->setIndent(5);

	buildPage(content, nav_bar, QString());
}

#define FIRST_PERIOD 0
#define SECOND_PERIOD 1

LoadDataContent::LoadDataContent(int _currency_decimals, int _rate_id)
{
	current_consumption = new QLabel;
	current_consumption->setText("---");
	current_consumption->setFont(bt_global::font->get(FontManager::SUBTITLE));
	current_value = 0;

	first_period = new Bann2Buttons;
	first_period->initBanner(QString(), bt_global::skin->getImage("empty_background"), bt_global::skin->getImage("reset"), tr("date/time"));
	first_period->setCentralText("---");
	first_period_value = 0;
	connect(first_period, SIGNAL(rightClicked()), &mapper, SLOT(map()));
	mapper.setMapping(first_period, FIRST_PERIOD);

	second_period = new Bann2Buttons;
	second_period->initBanner(QString(), bt_global::skin->getImage("empty_background"), bt_global::skin->getImage("reset"), tr("date/time"));
	second_period->setCentralText("---");
	second_period_value = 0;
	connect(second_period, SIGNAL(rightClicked()), &mapper, SLOT(map()));
	mapper.setMapping(second_period, SECOND_PERIOD);

#ifdef LAYOUT_TS_10
	first_period->layout()->setSpacing(5);
	second_period->layout()->setSpacing(5);
#endif

	connect(&mapper, SIGNAL(mapped(int)), SIGNAL(resetActuator(int)));

	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 0, 0, 0);
	main->setSpacing(5);
	main->addWidget(current_consumption, 0, Qt::AlignHCenter);
	main->addWidget(first_period);
	main->addWidget(second_period);

	rate_id = _rate_id;
	EnergyRates::energy_rates.loadRates();
	rate = EnergyRates::energy_rates.getRate(rate_id);
	connect(&EnergyRates::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));
	is_currency = false;
	currency_decimals = _currency_decimals;
}

void LoadDataContent::updatePeriodDate(int period, const QDate &date, const BtTime &time)
{
	QString time_str = QString("%1:%2").arg(time.hour()).arg(time.minute(), 2, 10, QChar('0'));
	QString text = DateConversions::formatDateConfig(date) + " " + time_str;
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

void LoadDataContent::updatePeriodValue(int period, qint64 new_value)
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
	double current = EnergyConversions::convertToRawData(current_value, EnergyConversions::ELECTRICITY);
	double period1 = EnergyConversions::convertToRawData(first_period_value, EnergyConversions::ELECTRICITY);
	double period2 = EnergyConversions::convertToRawData(second_period_value, EnergyConversions::ELECTRICITY);
	double current_kw = current; // to compute the cost
	int dec_current;
	QString unit_current;
	int dec = 3; // use always 3 decimals for the value
	QString unit_period = "kWh";

	// display values > 1 kW as kilowatts, lower values as watts
	if (current >= 1)
	{
		dec_current = 3;
		unit_current = "kW";
	}
	else
	{
		current = current * 1000;
		dec_current = 0;
		unit_current = "W";
	}

	if (is_currency)
	{
		current = EnergyConversions::convertToMoney(current_kw, rate.rate);
		period1 = EnergyConversions::convertToMoney(period1, rate.rate);
		period2 = EnergyConversions::convertToMoney(period2, rate.rate);
		dec = currency_decimals;
		unit_current = unit_period = rate.currency_symbol;
	}

	current_consumption->setText(QString("%1 %2").arg(loc.toString(current, 'f', dec_current)).arg(unit_current));
	first_period->setCentralText(QString("%1 %2").arg(loc.toString(period1, 'f', dec)).arg(unit_period));
	second_period->setCentralText(QString("%1 %2").arg(loc.toString(period2, 'f', dec)).arg(unit_period));
}


LoadDataPage::LoadDataPage(const QDomNode &config_node, LoadsDevice *d)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	QWidget *top = buildTitle(getDescriptionWithPriority(config_node));

	// measure unit here will always be kWh
	QString forward_button;
	int rate_id = -1;
	if (isRateEnabled(config_node))
	{
		forward_button = bt_global::skin->getImage("currency");
		rate_id = getRateId(config_node);
		EnergyRates::energy_rates.setRateDescription(rate_id, getTextChild(config_node, "descr"));
	}
	int decimals = getDecimals(config_node);
	content = new LoadDataContent(decimals, rate_id);

#ifdef LAYOUT_TS_3_5
	NavigationBar *nav_bar = new NavigationBar(forward_button);
	connect(nav_bar, SIGNAL(forwardClick()), content, SLOT(toggleCurrencyView()));
#else
	NavigationBar *nav_bar = new NavigationBar;
#endif

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	ConfirmationPage *confirm = new ConfirmationPage(tr("Text-Confirm-LoadDataPage"));
	// show pages correctly
	connect(content, SIGNAL(resetActuator(int)), confirm, SLOT(showPage()));
	connect(confirm, SIGNAL(Closed()), SLOT(showPage()));
	// these connects handle the logic
	connect(content, SIGNAL(resetActuator(int)), SLOT(resetRequested(int)));
	connect(confirm, SIGNAL(accept()), SLOT(reset()));
	reset_number = 0;

	dev = d;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

#ifdef LAYOUT_TS_3_5
	buildPage(content, nav_bar, "", 0, top);
#else
	QWidget *container = new QWidget;
	QVBoxLayout *vlayout = new QVBoxLayout;
#ifdef LAYOUT_TS_3_5
	vlayout->addWidget(content, 2);
#else
	vlayout->addWidget(content, 2, Qt::AlignHCenter);
#endif

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->addStretch(2);
	if (isRateEnabled(config_node))
	{
		BtButton *currency_button = new BtButton(bt_global::skin->getImage("currency_exchange"));
		connect(currency_button, SIGNAL(clicked()), content, SLOT(toggleCurrencyView()));
		buttons_layout->addWidget(currency_button, 1, Qt::AlignCenter);
	}
	vlayout->addLayout(buttons_layout, 1);
	container->setLayout(vlayout);

	buildPage(container, nav_bar, "", 0, top);
#endif
}

void LoadDataPage::showEvent(QShowEvent *e)
{
	dev->requestCurrentUpdateStart();
	dev->requestTotal(0);
	dev->requestTotal(1);
}

void LoadDataPage::hideEvent(QHideEvent *e)
{
	dev->requestCurrentUpdateStop();
}

void LoadDataPage::resetRequested(int which)
{
	reset_number = which;
}

void LoadDataPage::reset()
{
	dev->resetTotal(reset_number);
}

void LoadDataPage::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	// first, get the period, if any. We need it when we see date and consumption data
	int period = -1;
	if (values_list.contains(LoadsDevice::DIM_PERIOD))
		period = values_list[LoadsDevice::DIM_PERIOD].toInt();

	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case LoadsDevice::DIM_TOTAL:
			content->updatePeriodValue(period, it.value().toLongLong());
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

	QWidget *top = buildTitle(getDescriptionWithPriority(config_node));

#ifdef LAYOUT_TS_3_5
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	// TODO: cancel user selection?
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(sendDeactivateDevice()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));
#else
	NavigationBar *nav_bar = new NavigationBar();
#endif
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	deactivation_time = new DeactivationTime(BtTime(2, 30, 0));

#ifdef LAYOUT_TS_10
	QWidget *content = new QWidget;
	QHBoxLayout *banner_layout = new QHBoxLayout;
	banner_layout->addStretch();
	banner_layout->addWidget(deactivation_time);
	banner_layout->addStretch();

	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->addLayout(banner_layout);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->addStretch(2);

	BtButton *ok_button = new BtButton(bt_global::skin->getImage("ok"));
	connect(ok_button, SIGNAL(clicked()), SLOT(sendDeactivateDevice()));
	connect(ok_button, SIGNAL(clicked()), SIGNAL(Closed()));

	buttons_layout->addWidget(ok_button, 1, Qt::AlignCenter);

	vlayout->addLayout(buttons_layout);

	content->setLayout(vlayout);
	buildPage(content, nav_bar, "", 0, top);
#else
	buildPage(deactivation_time, nav_bar, "", 0, top);
#endif
	dev = d;
}

void DeactivationTimePage::sendDeactivateDevice()
{
	BtTime t = deactivation_time->currentTime();
	int off_time = t.hour() * 60 + t.minute();
	dev->forceOff(off_time);
}
