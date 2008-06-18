#include "poweramplifier.h"
#include "sottomenu.h"
#include "btlabel.h"
#include "main.h"

static const char *IMG_PLUS = IMG_PATH "btnplus.png";
static const char *IMG_MINUS = IMG_PATH "btnmin.png";
static const char *IMG_PRESET = IMG_PATH "preset.png";
static const char *IMG_TREBLE = IMG_PATH "louds.png";
static const char *IMG_BASS = IMG_PATH "lows.png";


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

	QColor *bg, *fg1, *fg2;
	readExtraConf(&bg, &fg1, &fg2);

	QPtrList<QString> icons;
	settings_page->addItemU((char)POWER_AMPLIFIER_PRESET, tr("Preset"), NULL, icons);
	settings_page->addItemU((char)POWER_AMPLIFIER_TREBLE, tr("Treble"), NULL, icons, 0, 0, *fg2);
	settings_page->addItemU((char)POWER_AMPLIFIER_BASS, tr("Bass"), NULL, icons, 0, 0, *fg2);
	settings_page->hide();
	connect(settings_page, SIGNAL(Closed()), settings_page, SLOT(hide()));

	delete bg;
	delete fg1;
	delete fg2;
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
	qDebug("PowerAmplifierPreset::PowerAmplifierPreset()");
	SetIcons(IMG_PLUS, IMG_MINUS, NULL, IMG_PRESET);
	preset = 0;
	num_preset = 20;
	connect(this, SIGNAL(sxClick()), SLOT(nextPreset()));
	connect(this, SIGNAL(dxClick()), SLOT(prevPreset()));
}

void PowerAmplifierPreset::showEvent(QShowEvent *event)
{
	showPreset();
	Draw();
}

void PowerAmplifierPreset::prevPreset()
{
	qDebug("PowerAmplifierPreset::prevPreset()");
	if (!preset)
		preset = num_preset;
	else
		--preset;
	showPreset();
	Draw();
}

void PowerAmplifierPreset::nextPreset()
{
	qDebug("PowerAmplifierPreset::nextPreset()");
	if (preset + 1 > num_preset)
		preset = 0;
	else
		++preset;
	showPreset();
	Draw();
}

void PowerAmplifierPreset::showPreset()
{
	QString desc;
	desc.sprintf("Preset %d", preset);
	SetTextU(desc.ascii());
}

/*****************************************************************
 ** PowerAmplifierTreble
 ****************************************************************/

PowerAmplifierTreble::PowerAmplifierTreble(QWidget *parent, const char *name, QColor SecondForeground)
 : bannOnOff2scr(parent, name)
{
	SetIcons(IMG_MINUS, IMG_PLUS, NULL, IMG_TREBLE);
	level = 0;
	qDebug("PowerAmplifierTreble::PowerAmplifierTreble()");
	connect(this, SIGNAL(sxClick()), SLOT(down()));
	connect(this, SIGNAL(dxClick()), SLOT(up()));
	secondary_fg = SecondForeground;
	showLevel();
}

void PowerAmplifierTreble::up()
{
	qDebug("PowerAmplifierTreble::up()");
	level += 10;
	showLevel();
	Draw();
}

void PowerAmplifierTreble::down()
{
	qDebug("PowerAmplifierTreble::down()");
	level -= 10;
	showLevel();
	Draw();
}

void PowerAmplifierTreble::showLevel()
{
	QString desc;
	desc.sprintf("%s%d", level > 0 ? "+" : "", level);
	SetSecondaryTextU(desc.ascii());
}

void PowerAmplifierTreble::setFGColor(QColor c)
{
	banner::setFGColor(c);
	SecondaryText->setPaletteForegroundColor(secondary_fg);
}

/*****************************************************************
 ** PowerAmplifierBass
 ****************************************************************/

PowerAmplifierBass::PowerAmplifierBass(QWidget *parent, const char *name, QColor SecondForeground)
 : bannOnOff2scr(parent, name)
{
	SetIcons(IMG_MINUS, IMG_PLUS, NULL, IMG_BASS);
	level = 0;
	qDebug("PowerAmplifierBass::PowerAmplifierTreble()");
	connect(this, SIGNAL(sxClick()), SLOT(down()));
	connect(this, SIGNAL(dxClick()), SLOT(up()));
	secondary_fg = SecondForeground;
	showLevel();
}

void PowerAmplifierBass::up()
{
	qDebug("PowerAmplifierBass::up()");
	level += 10;
	showLevel();
	Draw();
}

void PowerAmplifierBass::down()
{
	qDebug("PowerAmplifierBass::down()");
	level -= 10;
	showLevel();
	Draw();
}

void PowerAmplifierBass::showLevel()
{
	QString desc;
	desc.sprintf("%s%d", level > 0 ? "+" : "", level);
	SetSecondaryTextU(desc.ascii());
}

void PowerAmplifierBass::setFGColor(QColor c)
{
	banner::setFGColor(c);
	SecondaryText->setPaletteForegroundColor(secondary_fg);
}
