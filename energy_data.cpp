#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bann2_buttons.h" // bann2But, bannOnOff
#include "bann1_button.h" // bannPuls, bannTextOnImage
#include "energy_view.h" // EnergyView
#include "skinmanager.h" // bt_global::skin, SkinContext
#include "generic_functions.h" // setCfgValue
#include "devices_cache.h" // bt_global::devices_cache
#include "energy_device.h" // EnergyDevice

#include "bannfrecce.h"

#include <QVBoxLayout>
#include <QDomNode>
#include <QLocale>
#include <QDebug>

#include <assert.h>

// The language used for the floating point number
QLocale loc(QLocale::Italian);


EnergyData::EnergyData(const QDomNode &config_node)
{
	loadTypes(config_node);
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
			appendBanner(b); // to increase the serial number
			b->SetIcons(bt_global::skin->getImage("select"), bt_global::skin->getImage("currency_exchange"),
					QString(), bt_global::skin->getImage("energy_type"));

			ec_cost = new EnergyCost(type, b->getSerNum());
			b->connectSxButton(ec_cost);
		}
		else
		{
			b = new bannPuls(this);
			appendBanner(b);
			b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("energy_type"));
		}

		EnergyInterface *en_interf = new EnergyInterface(type);
		b->connectDxButton(en_interf);
		if (ec_cost)
		{
			connect(ec_cost, SIGNAL(consValueChanged(float)), en_interf, SLOT(changeConsRate(float)));
			connect(ec_cost, SIGNAL(prodValueChanged(float)), en_interf, SLOT(changeProdRate(float)));
		}
		b->setText(getTextChild(type, "descr"));
		b->setId(getTextChild(type, "id").toInt());
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}


EnergyCost::EnergyCost(const QDomNode &config_node, int serial)
{
	QDomElement currency_node = getElement(config_node, "currency");
	assert(!currency_node.isNull() && "currency node null!");

	delta = loc.toFloat(getTextChild(currency_node, "delta"));

	QString unit_measure = getTextChild(currency_node, "symbol") + "/" +
		getTextChild(config_node, "measure");

	n_decimal = getTextChild(currency_node, "n_decimal").toInt();

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
		b->SetIcons(bt_global::skin->getImage("minus"), bt_global::skin->getImage("plus"));

		rate = loc.toFloat(getTextChild(config_node, "rate"));
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
	temp_cons_rate -= delta;
	showValue(banner_cost, temp_cons_rate);
}

void EnergyCost::increaseCost()
{
	temp_cons_rate += delta;
	showValue(banner_cost, temp_cons_rate);
}

void EnergyCost::decreaseProd()
{
	temp_prod_rate -= delta;
	showValue(banner_prod, temp_prod_rate);
}

void EnergyCost::increaseProd()
{
	temp_prod_rate += delta;
	showValue(banner_prod, temp_prod_rate);
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
	data["cons/rate"] = loc.toString(cons_rate, 'f', 3);
	data["prod/rate"] = loc.toString(prod_rate, 'f', 3);
	setCfgValue(data, ENERGY_TYPE, serial_number);

	emit prodValueChanged(prod_rate);
	emit consValueChanged(cons_rate);
	emit Closed();
}


bool EnergyInterface::is_currency_view = false;

EnergyInterface::EnergyInterface(const QDomNode &config_node)
{
	loadItems(config_node);
	if (elencoBanner.size() == 1)
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void EnergyInterface::loadItems(const QDomNode &config_node)
{
	assert(bt_global::skin->hasContext() && "Skin context not set!");
	int mode = getTextChild(config_node, "mode").toInt();
	QString energy_type = getTextChild(config_node, "descr");
	measure = getTextChild(config_node, "measure");
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		bool is_currency_enabled = checkTypeForCurrency(getTextChild(item, "type"), config_node);

		QString currency;
		if (is_currency_enabled)
			currency = getElement(config_node, "currency/symbol").text();

		bool is_production = (getElement(item, "type").text().toInt() == 1);

		bannEnergyInterface *b = new bannEnergyInterface(this, currency, is_production);
		b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("empty"));
		QString addr = getTextChild(item, "address");
		next_page = new EnergyView(measure, energy_type, addr, mode, currency, is_production);
		b->connectDxButton(next_page);
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		b->setInternalText("---");
		b->setUnitMeasure(measure);

		views.append(next_page);

		device *dev = bt_global::devices_cache[get_device_key("18", addr)];
		connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
		connect(dev, SIGNAL(status_changed(const StatusList &)), b, SLOT(status_changed(const StatusList &)));
		conversion_factor = (mode == 1) ? 1000 : 1;
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		appendBanner(b);
	}
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

void EnergyInterface::showPage()
{
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

void EnergyInterface::toggleCurrencyView()
{
	is_currency_view = !is_currency_view;
}

bool EnergyInterface::isCurrencyView()
{
	return is_currency_view;
}
