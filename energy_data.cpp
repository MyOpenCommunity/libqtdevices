#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bann2_buttons.h" // bann2But, bannOnOff
#include "bann1_button.h" // bannPuls
#include "energy_view.h" // EnergyView
#include "skinmanager.h" // bt_global::skin, SkinContext

#include <QVBoxLayout>
#include <QDomNode>
#include <QDebug>

#include <assert.h>


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
		bannOnOff *b = new bannOnOff(this);
		b->SetIcons(bt_global::skin->getImage("select"), bt_global::skin->getImage("currency_exchange"),
					QString(), bt_global::skin->getImage("energy_type"));
		b->connectSxButton(new EnergyCost(type));
		b->connectDxButton(new EnergyInterface(type));
		b->setText(getTextChild(type, "descr"));
		b->setId(getTextChild(type, "id").toInt());
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		appendBanner(b);
	}
}


EnergyCost::EnergyCost(const QDomNode &config_node)
{
	QDomElement currency_node = getElement(config_node, "currency");
	assert(!currency_node.isNull() && "currency node null!");

	delta = getTextChild(currency_node, "delta").toInt();

	QString unit_measure = getTextChild(currency_node, "symbol") + "/" +
		getTextChild(config_node, "measure");

	int n_decimal = getTextChild(currency_node, "n_decimal").toInt();

	addBanner(getElement(config_node, "cons"), tr("Consumption") + " " + unit_measure, n_decimal);
	addBanner(getElement(config_node, "prod"), tr("Production") + " " + unit_measure, n_decimal);
	addBackButton();
}

void EnergyCost::addBanner(const QDomNode &config_node, QString desc, int n_decimal)
{
	if (!config_node.isNull() && getTextChild(config_node, "ab").toInt() == 1)
	{
		bann2ButLab *b = new bann2ButLab(this);
		b->SetIcons(bt_global::skin->getImage("minus"), bt_global::skin->getImage("plus"));
		b->setText(QString::number(getTextChild(config_node, "rate").toFloat(), 'f', n_decimal));
		b->setSecondaryText(desc);
		b->Draw();
		main_layout->addWidget(b);
	}
}


EnergyInterface::EnergyInterface(const QDomNode &config_node) : sottoMenu(0, 1)
{
	loadItems(config_node);
}

void EnergyInterface::loadItems(const QDomNode &config_node)
{
	assert(bt_global::skin->hasContext() && "Skin context not set!");
	QString energy_type = getTextChild(config_node, "descr");
	QString measure = getTextChild(config_node, "measure");
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		bannPuls *b = new bannPuls(this);
		b->SetIcons(bt_global::skin->getImage("select"), QString(), bt_global::skin->getImage("empty"));
		b->connectDxButton(new EnergyView(measure, energy_type, getTextChild(item, "address")));
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		appendBanner(b);
	}
}

