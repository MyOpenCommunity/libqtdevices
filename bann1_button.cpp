#include "bann1_button.h"
#include "sottomenu.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, ..
#include "titlelabel.h" // TextOnImageLabel
#include "skinmanager.h" // bt_global::skin
#include "energy_data.h" // EnergyInterface

#include <QLocale>

#define BUT_DIM 60
#define BUTONDX_H_SCRITTA 20
#define BANNONICONS_ICON_DIM 60
#define BANON2SCR_TEXT1_DIM_X 100
#define BANON2SCR_TEXT1_DIM_Y 60
#define BUTON2SCR_ICON_DIM_X 40
#define BUTON2SCR_ICON_DIM_Y 60
#define BANNBUT2ICON_TEXT_DIM_X 240
#define BANNBUT2ICON_TEXT_DIM_Y 20
#define BANNBUT2ICON_ICON_DIM_X 60
#define BANNBUT2ICON_ICON_DIM_Y 60

namespace
{
	QLocale loc(QLocale::Italian);
}

bannPuls::bannPuls(QWidget *parent) : banner(parent)
{
	addItem(BUT1, MAX_WIDTH-BANPULS_BUT_DIM, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	addItem(ICON,BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(TEXT, 0 , BANPULS_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BANPULS_BUT_DIM);

	connect(this,SIGNAL(sxClick()),this,SIGNAL(click()));
	connect(this,SIGNAL(sxPressed()),this,SIGNAL(pressed()));
	connect(this,SIGNAL(sxReleased()),this,SIGNAL(released()));
}


bannSimple::bannSimple(sottoMenu *parent, QString icon, Page *page) : banner(parent)
{
	// This banner uses a dx button but shows it in central position.
	addItem(BUT1, (MAX_WIDTH - BANPULS_BUT_DIM) / 2, 0,  BANPULS_BUT_DIM ,BANPULS_BUT_DIM);
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnDx::bannOnDx(sottoMenu *parent, QString icon, Page *page) : banner(parent)
{
	unsigned char nr = parent->getNumRig();

	addItem(BUT1, MAX_WIDTH-BUT_DIM, (((MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr)-BUT_DIM)/2 ,
		BUT_DIM ,BUT_DIM);
	addItem(TEXT , 0, 0 , MAX_WIDTH-BUT_DIM,
		((MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE)/nr));
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);

	connectDxButton(page);
	Draw();
}


bannOnSx::bannOnSx(sottoMenu *parent, QString icon) : banner(parent)
{
	unsigned char nr = parent->getNumRig();
	addItem(BUT1, 0, (((MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE)/nr) - BUT_DIM)/2, BUT_DIM,
		BUT_DIM);
	addItem(TEXT ,BUT_DIM, 0, MAX_WIDTH - BUT_DIM, ((MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE)/nr));
	connect(this, SIGNAL(sxClick()), this, SIGNAL(click()));

	if (!icon.isEmpty())
		SetIcons(icon, 1);
	Draw();
}

BtButton *bannOnSx::getButton()
{
	return sxButton;
}


bannOnIcons::bannOnIcons(QWidget *parent) : banner(parent)
{
	addItem(BUT1 , 0 , 0 , BUT_DIM, BUT_DIM);
	addItem(TEXT , 0 , BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE-BUT_DIM);
	addItem(ICON , BUT_DIM , 0 , BUT_DIM , BUT_DIM);
	addItem(ICON2 , 2*BUT_DIM , 0 , BUT_DIM , BUT_DIM);
}


bannOn2scr::bannOn2scr(QWidget *parent) : banner(parent)
{
	int x = 0, y = 0;
	addItem(TEXT2, x, y, BANON2SCR_TEXT1_DIM_X, BANON2SCR_TEXT1_DIM_Y);
	x = BANON2SCR_TEXT1_DIM_X;
	addItem(ICON, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
	x += BUTON2SCR_ICON_DIM_X;
	addItem(ICON2, x, y, BUTON2SCR_ICON_DIM_X, BUTON2SCR_ICON_DIM_Y);
	x += BUTON2SCR_ICON_DIM_X;
	addItem(BUT2, x, y, BUT_DIM, BUT_DIM);
	y = BUT_DIM;
	x = 0;
	addItem(TEXT, x, y, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE - BUT_DIM);
}


bannBut2Icon::bannBut2Icon(QWidget *parent) : banner(parent)
{
	addItem(BUT1 , MAX_WIDTH - BUT_DIM, 0, BUT_DIM, BUT_DIM);
	addItem(ICON, BUT_DIM, 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(ICON2, BUT_DIM + BANNBUT2ICON_ICON_DIM_X , 0, BANNBUT2ICON_ICON_DIM_X, BANNBUT2ICON_ICON_DIM_Y);
	addItem(TEXT, 0, BUT_DIM, MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE - BUT_DIM);
}


bannTextOnImage::bannTextOnImage(QWidget *parent, const QString &text) : banner(parent)
{
	label = new TextOnImageLabel(this, text);
	label->setBackgroundImage(bt_global::skin->getImage("empty_icon"));
	label->setGeometry(BANPULS_BUT_DIM, 0,  BANPULS_ICON_DIM_X ,BANPULS_ICON_DIM_Y);
	addItem(BUT1, MAX_WIDTH-BUT_DIM, 0, BUT_DIM ,BUT_DIM);
	addItem(TEXT, 0, BUT_DIM, MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE - BUT_DIM);
	SetIcons(bt_global::skin->getImage("forward"), 1);
}

void bannTextOnImage::setInternalText(const QString &text)
{
	label->setInternalText(text);
}


bannEnergyInterface::bannEnergyInterface(QWidget *parent, const QString &_currency_symbol,
	bool is_prod, bool is_ele) : bannTextOnImage(parent)
{
	currency_symbol = _currency_symbol;
	is_production = is_prod;
	is_electricity = is_ele;
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
	if (device_value)
	{
		float data = EnergyConversions::convertToRawData(device_value,
			is_electricity ? EnergyConversions::ELECTRICITY_CURRENT : EnergyConversions::DEFAULT_ENERGY);
		float factor = is_production ? prod_factor : cons_factor;
		QString str = measure;
		if (EnergyInterface::isCurrencyView())
		{
			data = EnergyConversions::convertToMoney(data, factor);
			str = currency_symbol;
		}

		setInternalText(QString("%1 %2").arg(loc.toString(data, 'f', 3)).arg(str));
	}
	else
		setInternalText("---");
}

void bannEnergyInterface::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == EnergyDevice::DIM_CURRENT)
		{
			device_value = it.value().toInt();
			updateText();
			// TODO: is this necessary?
			Draw();
			break;
		}
		++it;
	}
}
