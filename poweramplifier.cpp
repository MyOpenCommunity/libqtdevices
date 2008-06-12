#include "poweramplifier.h"
#include "sottomenu.h"

static const char *IMG_PLUS = IMG_PATH "btnplus.png";
static const char *IMG_MINUS = IMG_PATH "btnmin.png";
static const char *IMG_PRESET = IMG_PATH "preset.png";

/*****************************************************************
 ** PowerAmplifier
 ****************************************************************/

PowerAmplifier::PowerAmplifier(QWidget *parent, const char *name, char* indirizzo, char* onIcon, char* offIcon, char *onAmpl, char *offAmpl, char* settingIcon)
: bannRegolaz(parent, name)
{
	qDebug("PowerAmplifier::PowerAmplifier()");
	setRange(1,9);
	setValue(1);
	SetIcons(settingIcon, offIcon ,onAmpl, offAmpl,(char)1);
	qDebug("%s - %s - %s - %s - %s", onIcon, offIcon, onAmpl, offAmpl, settingIcon);
	setAddress(indirizzo);
	connect(this, SIGNAL(sxClick()), SLOT(showSettings()));
	connect(this, SIGNAL(dxClick()), SLOT(toggleStatus()));
	connect(this, SIGNAL(cdxClick()), SLOT(turnUp()));
	connect(this, SIGNAL(csxClick()), SLOT(turnDown()));

	off_icon = offIcon;
	on_icon = onIcon;
	status = false;
	settings_page = new sottoMenu(NULL, "PowerAmplifierSettings");

	QPtrList<QString> icons;
	settings_page->addItemU((char)POWER_AMPLIFIER_PRESET, "Preset", NULL, icons);
	settings_page->hide();
	connect(settings_page, SIGNAL(Closed()), settings_page, SLOT(hide()));
}

void PowerAmplifier::setBGColor(QColor c)
{
	banner::setBGColor(c);
	settings_page->setAllBGColor(c);
}

void PowerAmplifier::setFGColor(QColor c)
{
	banner::setFGColor(c);
	settings_page->setAllFGColor(c);
}

void PowerAmplifier::showSettings()
{
	settings_page->show();
}

void PowerAmplifier::toggleStatus()
{
	SetIcons(1, status ? off_icon.ascii() : on_icon.ascii());
	status = !status;
	Draw();
}

void PowerAmplifier::turnUp()
{
	qDebug("PowerAmplifier::turnUp()");
}

void PowerAmplifier::turnDown()
{
	qDebug("PowerAmplifier::turnDown()");
}

/*****************************************************************
 ** PowerAmplifierPreset
 ****************************************************************/

PowerAmplifierPreset::PowerAmplifierPreset(QWidget *parent, const char *name)
 : bannOnOff(parent, name)
 {
	SetIcons(IMG_PLUS, IMG_MINUS, NULL, IMG_PRESET);
	qDebug("PowerAmplifierPreset::PowerAmplifierPreset()");
	connect(this, SIGNAL(sxClick()), SLOT(nextPreset()));
	connect(this, SIGNAL(dxClick()), SLOT(prevPreset()));
 }

void PowerAmplifierPreset::prevPreset()
{
	qDebug("PowerAmplifierPreset::prevPreset()");
}

void PowerAmplifierPreset::nextPreset()
{
	qDebug("PowerAmplifierPreset::nextPreset()");
}
