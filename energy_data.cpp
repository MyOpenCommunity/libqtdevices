#include "energy_data.h"
#include "main.h" // IMG_PATH
#include "xml_functions.h" // getChildren, getTextChild
#include "bannonoff.h"
#include "bann2but.h"

#include <QDomNode>


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
		int mode = getTextChild(item, "mode").toInt();
		bannOnOff *b = new bannOnOff(this);
		b->SetIcons(IMG_SELECT, IMG_SETTINGS, NULL, IMG_PRESET);
		b->connectSxButton(new EnergyCost);
		b->setText(getTextChild(item, "descr"));
		b->setId(getTextChild(item, "id").toInt());
		appendBanner(b);
	}
}


EnergyCost::EnergyCost() : sottoMenu(0, 1)
{
	bann2But *b = new bann2But(this);
	b->SetIcons(IMG_MINUS, IMG_PLUS);
	b->setText("0,167");
	appendBanner(b);
}


