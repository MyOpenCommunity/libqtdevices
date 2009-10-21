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
#include "content_widget.h"

#include <QVBoxLayout>
#include <QDomNode>
#include <QLocale>
#include <QDebug>

#include <math.h> // pow

// The language used for the floating point number
QLocale loc(QLocale::Italian);


EnergyData::EnergyData(const QDomNode &config_node)
{
	buildPage();
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
	SkinContext context(getTextChild(config_node, "cid").toInt());
	foreach (const QDomNode& type, getChildren(config_node, "energy_type"))
	{
		SkinContext cont(getTextChild(type, "cid").toInt());
		banner *b;
		EnergyCost *ec_cost = 0;
		if (getElement(type, "currency/ab").text().toInt() == 1)
		{
			b = new bannOnOff(this);
			content_widget->appendBanner(b); // to increase the serial number
			b->SetIcons(bt_global::skin->getImage("select"), bt_global::skin->getImage("currency_exchange"),
					QString(), bt_global::skin->getImage("energy_type"));

			ec_cost = new EnergyCost(type, b->getSerNum());
			b->connectSxButton(ec_cost);
		}
		else
		{
			b = new bannPuls(this);
			content_widget->appendBanner(b);
			b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("energy_type"));
		}

		EnergyInterface *en_interf = new EnergyInterface(type);
		interfaces.push_back(en_interf);
		b->connectDxButton(en_interf);
		if (ec_cost)
		{
			connect(ec_cost, SIGNAL(consValueChanged(float)), en_interf, SLOT(changeConsRate(float)));
			connect(ec_cost, SIGNAL(prodValueChanged(float)), en_interf, SLOT(changeProdRate(float)));
		}
		b->setText(getTextChild(type, "descr"));
		b->setId(getTextChild(type, "id").toInt());
		b->Draw();
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}


EnergyCost::EnergyCost(const QDomNode &config_node, int serial)
{
	QDomElement currency_node = getElement(config_node, "currency");
	Q_ASSERT_X(!currency_node.isNull(), "EnergyCost::EnergyCost", "currency node null!");

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
	Q_ASSERT_X(bt_global::skin->hasContext() , "EnergyInterface::loadItems", "Skin context not set!");
	int mode = getTextChild(config_node, "mode").toInt();
	QString energy_type = getTextChild(config_node, "descr");
	QString measure = getTextChild(config_node, "measure");
	bool show_currency_button = false;
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		bool is_currency_enabled = checkTypeForCurrency(getTextChild(item, "type"), config_node);
		// check if any of the interfaces have currency enabled
		show_currency_button |= is_currency_enabled;

		QString currency;
		if (is_currency_enabled)
			currency = getElement(config_node, "currency/symbol").text();

		int n_decimal = getElement(config_node, "currency/n_decimal").text().toInt();
		bool is_production = (getElement(item, "type").text().toInt() == 1);

		bannEnergyInterface *b = new bannEnergyInterface(this, currency, n_decimal, is_production, mode == 1);
		b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("empty"));
		QString addr = getTextChild(item, "address");
		next_page = new EnergyView(measure, energy_type, addr, mode, currency, n_decimal, is_production);
		b->connectDxButton(next_page);
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		b->setInternalText("---");
		b->setUnitMeasure(measure);

		// set production/consumption rates
		QDomElement prod_node = getElement(config_node, "prod/rate");
		if (!prod_node.isNull())
		{
			float rate = loc.toFloat(prod_node.text());
			next_page->setProdFactor(rate);
			b->setProdFactor(rate);
		}
		QDomElement cons_node = getElement(config_node, "cons/rate");
		if (!cons_node.isNull())
		{
			float rate = loc.toFloat(cons_node.text());
			next_page->setConsFactor(rate);
			b->setConsFactor(rate);
		}

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

void EnergyInterface::changeConsRate(float cons)
{
	for (int i = 0; i < views.size(); ++i)
		views[i]->setConsFactor(cons);
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		bannEnergyInterface *b = static_cast<bannEnergyInterface*>(elencoBanner[i]);
		b->setConsFactor(cons);
	}
}

void EnergyInterface::changeProdRate(float prod)
{
	for (int i = 0; i < views.size(); ++i)
		views[i]->setProdFactor(prod);
	for (int i = 0; i < elencoBanner.size(); ++i)
	{
		bannEnergyInterface *b = static_cast<bannEnergyInterface*>(elencoBanner[i]);
		b->setProdFactor(prod);
	}
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


bannEnergyInterface::bannEnergyInterface(QWidget *parent, const QString &_currency_symbol,
	int n_dec, bool is_prod, bool is_ele) : bannTextOnImage(parent)
{
	currency_symbol = _currency_symbol;
	is_production = is_prod;
	is_electricity = is_ele;
	n_decimal = n_dec;
	device_value = 0;
}

void bannEnergyInterface::setProdFactor(float prod)
{
	prod_factor = prod;
	updateText();
}

void bannEnergyInterface::setConsFactor(float cons)
{
	cons_factor = cons;
	updateText();
}

void bannEnergyInterface::setType(EnergyFactorType t)
{
	type = t;
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
		float factor = is_production ? prod_factor : cons_factor;
		QString str = measure;
		if (EnergyInterface::isCurrencyView())
		{
			if (!currency_symbol.isNull())
			{
				data = EnergyConversions::convertToMoney(data, factor);
				str = currency_symbol;
				text = QString("%1 %2").arg(loc.toString(data, 'f', n_decimal)).arg(str);
			}
		}
		else
			text = QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(str);
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
