#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bann2_buttons.h" // bann2But, bannOnOff
#include "energy_view.h" // EnergyView
#include "skinmanager.h" // bt_global::skin, SkinContext
#include "generic_functions.h" // setCfgValue
#include "devices_cache.h" // bt_global::devices_cache
#include "energy_device.h" // EnergyDevice
#include "bannfrecce.h"
#include "btmain.h" // bt_global::btmain
#include "energy_rates.h"
#include "bann_energy.h" // bannEnergyInterface
#include "bann1_button.h" // BannSinglePuls

#include <QVBoxLayout>
#include <QDomNode>
#include <QLocale>
#include <QDebug>

#include <assert.h>
#include <math.h>

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


EnergyData::EnergyData(const QDomNode &config_node)
{
	loadTypes(config_node);
	connect(bt_global::btmain, SIGNAL(resettimer()), SLOT(systemTimeChanged()));
	connect(&day_timer, SIGNAL(timeout()), SLOT(updateDayTimer()));
	connect(&day_timer, SIGNAL(timeout()), SLOT(updateInterfaces()));
	day_timer.setSingleShot(true);
}

void EnergyData::updateDayTimer()
{
	const int msecs_in_a_day = 86400000;
	day_timer.stop();
	// msecsTo() below returns always negative
	int next_day_msecs = msecs_in_a_day + QTime::currentTime().msecsTo(QTime(0, 0, 0));
	day_timer.start(next_day_msecs);
}

void EnergyData::updateInterfaces()
{
	foreach(EnergyInterface *en_int, interfaces)
		en_int->systemTimeChanged();
}

void EnergyData::systemTimeChanged()
{
	updateDayTimer();
	updateInterfaces();
}

void EnergyData::loadTypes(const QDomNode &config_node)
{
	bt_global::energy_rates.loadRates();

	SkinContext context(getTextChild(config_node, "cid").toInt());
	foreach (const QDomNode& type, getChildren(config_node, "energy_type"))
	{
		SkinContext cont(getTextChild(type, "cid").toInt());
		BannSinglePuls *b = new BannSinglePuls(0);

		b->initBanner(bt_global::skin->getImage("select"), bt_global::skin->getImage("energy_type"),
			      getTextChild(type, "descr"));
		page_content->appendBanner(b);

		EnergyInterface *en_interf = new EnergyInterface(type);
		interfaces.push_back(en_interf);
		b->connectRightButton(en_interf);

		// TODO energy propagate rate changes to banner/pages

		b->setText(getTextChild(type, "descr"));
		b->setId(getTextChild(type, "id").toInt());
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}


EnergyCost::EnergyCost(const QDomNode &config_node, int serial)
{
	QDomElement currency_node = getElement(config_node, "currency");
	assert(!currency_node.isNull() && "currency node null!");

	bool ok = true;
	delta = loc.toFloat(getTextChild(currency_node, "delta"), &ok);
	if (!ok)
		qFatal("Delta is in wrong format, you should use ',' instead of '.'");

	QString unit_measure = getTextChild(currency_node, "symbol") + "/" +
		getTextChild(config_node, "measure");
	if((getTextChild(config_node, "mode").toInt()  == 1) || (getTextChild(config_node, "mode").toInt()  == 5))
		unit_measure += "h";

	n_decimal = getTextChild(currency_node, "n_decimal").toUInt();
	n_integer = getTextChild(currency_node, "n_integer").toUInt();

	banner_cost = addBanner(getElement(config_node, "cons"), tr("Consumption") + " " + unit_measure, cons_rate);
	banner_prod = addBanner(getElement(config_node, "prod"), tr("Production") + " " + unit_measure, prod_rate);

	if (banner_cost)
	{
		connect(banner_cost, SIGNAL(sxClick()), SLOT(decreaseCost()));
		connect(banner_cost, SIGNAL(dxClick()), SLOT(increaseCost()));
		temp_cons_rate = cons_rate;
	}

	if (banner_prod)
	{
		connect(banner_prod, SIGNAL(sxClick()), SLOT(decreaseProd()));
		connect(banner_prod, SIGNAL(dxClick()), SLOT(increaseProd()));
		temp_prod_rate = prod_rate;
	}

	main_layout->addStretch();
	bannFrecce *nav_bar = new bannFrecce(this, 10, bt_global::skin->getImage("ok"));
	connect(nav_bar, SIGNAL(backClick()), SLOT(closePage()));
	connect(nav_bar, SIGNAL(dxClick()), SLOT(saveCostAndProd()));
	main_layout->addWidget(nav_bar);
	serial_number = serial;
}

void EnergyCost::showValue(banner *b, float value)
{
	if (b)
	{
		b->setText(loc.toString(value, 'f', n_decimal));
		b->Draw();
	}
}

banner *EnergyCost::addBanner(const QDomNode &config_node, QString desc, float& rate)
{
	if (!config_node.isNull() && getTextChild(config_node, "ab").toInt() == 1)
	{
		bann2ButLab *b = new bann2ButLab(this);
		b->setAutoRepeat();
		b->SetIcons(bt_global::skin->getImage("minus"), bt_global::skin->getImage("plus"));

		bool ok;
		rate = loc.toFloat(getTextChild(config_node, "rate"), &ok);
		if (!ok)
			qFatal("Rate is in wrong format, you should use ',' instead of '.'");
		showValue(b, rate);
		b->setSecondaryText(desc);
		b->Draw();
		main_layout->addWidget(b);
		return b;
	}
	return 0;
}

void EnergyCost::closePage()
{
	// rollback the changes
	temp_cons_rate = cons_rate;
	temp_prod_rate = prod_rate;
	// refresh the values visualized
	showValue(banner_cost, temp_cons_rate);
	showValue(banner_prod, temp_prod_rate);
	emit Closed();
}

void EnergyCost::decreaseCost()
{
	if (temp_cons_rate - delta >= delta)
	{
		temp_cons_rate -= delta;
		showValue(banner_cost, temp_cons_rate);
	}
}

void EnergyCost::increaseCost()
{
	if (temp_cons_rate + delta < pow(10, n_integer))
	{
		temp_cons_rate += delta;
		showValue(banner_cost, temp_cons_rate);
	}
}

void EnergyCost::decreaseProd()
{
	if (temp_prod_rate - delta >= delta)
	{
		temp_prod_rate -= delta;
		showValue(banner_prod, temp_prod_rate);
	}
}

void EnergyCost::increaseProd()
{
	if (temp_prod_rate + delta < pow(10, n_integer))
	{
		temp_prod_rate += delta;
		showValue(banner_prod, temp_prod_rate);
	}
}

void EnergyCost::saveCostAndProd()
{
	// save the cost and prod
	cons_rate = temp_cons_rate;
	prod_rate = temp_prod_rate;
	// refresh the values visualized
	showValue(banner_cost, temp_cons_rate);
	showValue(banner_prod, temp_prod_rate);

	QMap<QString, QString> data;
	if (banner_cost)
		data["cons/rate"] = loc.toString(cons_rate, 'f', n_decimal);
	if (banner_prod)
		data["prod/rate"] = loc.toString(prod_rate, 'f', n_decimal);
	setCfgValue(data, ENERGY_TYPE, serial_number);

	emit prodValueChanged(prod_rate);
	emit consValueChanged(cons_rate);
	emit Closed();
}


bool EnergyInterface::is_currency_view = false;

EnergyInterface::EnergyInterface(const QDomNode &config_node)
{
	is_any_interface_enabled = false;
	loadItems(config_node);
	if (elencoBanner.size() == 1)
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void EnergyInterface::loadItems(const QDomNode &config_node)
{
	assert(bt_global::skin->hasContext() && "Skin context not set!");
	QString energy_type = getTextChild(config_node, "descr");
	bool show_currency_button = false;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int mode = getTextChild(item, "mode").toInt();
		QString measure = getTextChild(item, "measure");

		bool is_currency_enabled = getElement(item, "rate/ab").text().toInt();
		int rate_id = is_currency_enabled ? getElement(item, "rate/rate_id").text().toInt() : -1;

		// check if any of the interfaces have currency enabled
		show_currency_button |= is_currency_enabled;

		bannEnergyInterface *b = new bannEnergyInterface(this, rate_id, mode == 1);
		b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("empty"));
		QString addr = getTextChild(item, "address");
		next_page = new EnergyView(measure, energy_type, addr, mode, rate_id);
		b->connectDxButton(next_page);
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		b->setInternalText("---");
		b->setUnitMeasure(measure);

		views.append(next_page);

		device *dev = bt_global::devices_cache[get_device_key("18", addr)];
		connect(dev, SIGNAL(status_changed(const StatusList &)), b, SLOT(status_changed(const StatusList &)));
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		appendBanner(b);
	}

	if (show_currency_button)
	{
		is_any_interface_enabled = show_currency_button;
		setNavBarMode(10, bt_global::skin->getImage("currency"));
		connect(bannNavigazione, SIGNAL(dxClick()), SLOT(toggleCurrency()));
	}
}

void EnergyInterface::systemTimeChanged()
{
	for (int i = 0; i < views.size(); ++i)
		views[i]->systemTimeChanged();
}

bool EnergyInterface::checkTypeForCurrency(const QString &type, const QDomNode &conf)
{
	bool currency_enabled = (getElement(conf, "currency/ab").text().toInt() == 1);
	QString node_path;
	switch (type.toInt())
	{
	case 0: //consumption
		node_path = "cons/ab";
		break;
	case 1: // production
		node_path = "prod/ab";
		break;
	}
	QDomElement node = getElement(conf, node_path);
	if (!node.isNull())
	{
		bool cons_ab = (node.text().toInt() == 1);
		return currency_enabled && cons_ab;
	}
	else
		return false;
}

void EnergyInterface::updateBanners()
{
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		bannEnergyInterface *b = static_cast<bannEnergyInterface*>(elencoBanner[i]);
		b->updateText();
	}
}

void EnergyInterface::showPage()
{
	// restore visualization of raw data if we aren't enabled
	if (isCurrencyView() && !is_any_interface_enabled)
		toggleCurrencyView();

	updateBanners();
	if (elencoBanner.size() == 1)
		next_page->showPage();
	else
		Page::showPage();
}

void EnergyInterface::toggleCurrency()
{
	toggleCurrencyView();
	updateBanners();
}

void EnergyInterface::toggleCurrencyView()
{
	is_currency_view = !is_currency_view;
}

bool EnergyInterface::isCurrencyView()
{
	return is_currency_view;
}
