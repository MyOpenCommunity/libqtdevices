#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bann2_buttons.h" // bann2But, bannOnOff
#include "bann1_button.h" // bannPuls
#include "energy_view.h" // EnergyView
#include "skinmanager.h" // bt_global::skin, SkinContext
#include "generic_functions.h" // setCfgValue

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
		if (getElement(type, "currency/ab").text().toInt() == 1)
		{
			b = new bannOnOff(this);
			appendBanner(b); // to increase the serial number
			b->SetIcons(bt_global::skin->getImage("select"), bt_global::skin->getImage("currency_exchange"),
					QString(), bt_global::skin->getImage("energy_type"));

			b->connectSxButton(new EnergyCost(type, b->getSerNum()));
		}
		else
		{
			b = new bannPuls(this);
			appendBanner(b);
			b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("energy_type"));
		}

		b->connectDxButton(new EnergyInterface(type));
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
	// TODO: save values in the xml conf file.
	emit Closed();
}


EnergyInterface::EnergyInterface(const QDomNode &config_node) : sottoMenu(0, 1)
{
	loadItems(config_node);
}

void EnergyInterface::loadItems(const QDomNode &config_node)
{
	assert(bt_global::skin->hasContext() && "Skin context not set!");
	int mode = getTextChild(config_node, "mode").toInt();
	QString energy_type = getTextChild(config_node, "descr");
	QString measure = getTextChild(config_node, "measure");
	bool currency_enabled = (getElement(config_node, "currency/ab").text().toInt() == 1);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		bannPuls *b = new bannPuls(this);
		b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("empty"));
		b->connectDxButton(new EnergyView(measure, energy_type, getTextChild(item, "address"), mode, currency_enabled));
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		appendBanner(b);
	}
}

