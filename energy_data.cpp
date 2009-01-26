#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bannonoff.h"
#include "bann2but.h"
#include "bann1_button.h" // bannPuls

#include <QDomNode>

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


EnergyCost::EnergyCost(const QDomNode &config_node) : sottoMenu(0, 1)
{
	QDomElement currency_node = getElement(config_node, "currency");
	assert(!currency_node.isNull() && "currency node null!");

	QString unit_measure = getTextChild(currency_node, "symbol") + "/" +
		getTextChild(config_node, "measure");

	int num_decimal = getTextChild(currency_node, "n_decimal").toInt();
	int num_integer = getTextChild(currency_node, "n_integer").toInt();
	int delta = getTextChild(currency_node, "delta").toInt();

	QDomElement cost_node = getElement(config_node, "cost");
	if (!cost_node.isNull() && getTextChild(cost_node, "ab").toInt() == 1)
	{
		bann2But *b = new bann2But(this);
		b->SetIcons(IMG_MINUS, IMG_PLUS);
		int rate = getTextChild(cost_node, "rate").toInt();
		b->setText("0,167");
		b->setSecondaryText(tr("Consumption") + " " + unit_measure);
		appendBanner(b);
	}

	QDomElement prod_node = getElement(config_node, "prod");
	if (!prod_node.isNull() && getTextChild(prod_node, "ab").toInt() == 1)
	{
		bann2But *b = new bann2But(this);
		b->SetIcons(IMG_MINUS, IMG_PLUS);
		b->setText("0,154");
		b->setSecondaryText(tr("Production") + " " + unit_measure);
		appendBanner(b);
	}
}


EnergyInterface::EnergyInterface(const QDomNode &config_node) :
	sottoMenu(0, 1)
{
	loadItems(config_node);
}

void EnergyInterface::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		bannPuls *b = new bannPuls(this);
		b->SetIcons(IMG_SELECT, QString(), IMG_PRESET);
		// TODO: change this
		//b->connectDxButton(new EnergyCost());
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		// TODO: is address same as 'where'?
		QString address = getTextChild(item, "address");
		int type = getTextChild(item, "type").toInt();
		appendBanner(b);
	}
}

