#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bann2_buttons.h" // bann2But, bannOnOff
#include "bann1_button.h" // bannPuls
#include "energy_view.h" // EnergyView

#include <QVBoxLayout>
#include <QDomNode>
#include <QDebug>

#include <assert.h>

static const char *IMG_SETTINGS = IMG_PATH "setscen.png";
static const char *IMG_SELECT = IMG_PATH "arrrg.png";
static const char *IMG_PRESET = IMG_PATH "preset.png";
static const char *IMG_PLUS = IMG_PATH "btnplus.png";
static const char *IMG_MINUS = IMG_PATH "btnmin.png";


EnergyData::EnergyData(const QDomNode &config_node)
{
	loadTypes(config_node);
}

void EnergyData::loadTypes(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "energy_type"))
	{
		bannOnOff *b = new bannOnOff(this);
		b->SetIcons(IMG_SELECT, IMG_SETTINGS, QString(), IMG_PRESET);
		b->connectSxButton(new EnergyCost(item));
		b->connectDxButton(new EnergyInterface(item));
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
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
		b->SetIcons(IMG_MINUS, IMG_PLUS);
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
	QString energy_type = getTextChild(config_node, "descr");
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		bannPuls *b = new bannPuls(this);
		b->SetIcons(IMG_SELECT, QString(), IMG_PRESET);
		b->connectDxButton(new EnergyView(energy_type));
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		// TODO: is address same as 'where'?
		QString address = getTextChild(item, "address");
		int type = getTextChild(item, "type").toInt();
		appendBanner(b);
	}
}

