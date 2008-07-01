#include "poweramplifier.h"
#include "device_cache.h" // btouch_device_cache
#include "sottomenu.h"
#include "btlabel.h"
#include "poweramplifier_device.h"
#include "main.h"

#include <qregexp.h>
#include <stdlib.h>

static const char *IMG_PLUS = IMG_PATH "btnplus.png";
static const char *IMG_MINUS = IMG_PATH "btnmin.png";
static const char *IMG_PRESET = IMG_PATH "preset.png";
static const char *IMG_TREBLE = IMG_PATH "louds.png";
static const char *IMG_BASS = IMG_PATH "lows.png";
static const char *IMG_BALANCE = IMG_PATH "balance_0.png";
static const char *IMG_BALANCE_SX = IMG_PATH "balance_sx.png";
static const char *IMG_BALANCE_DX = IMG_PATH "balance_dx.png";
static const char *IMG_LOUD_ON = IMG_PATH "loud.png";
static const char *IMG_LOUD_OFF = IMG_PATH "loud_off.png";

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
	setAddress(indirizzo);
	dev = static_cast<poweramplifier_device*>(btouch_device_cache.get_poweramplifier_device(getAddress()));
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
	settings_page->addItemU((char)POWER_AMPLIFIER_BALANCE, tr("Balance"), NULL, icons, 0, 0, *fg2);
	settings_page->addItemU((char)POWER_AMPLIFIER_LOUD, tr("Loud"), NULL, icons, 0, 0, *fg2);
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
	fillPresetDesc();
	connect(this, SIGNAL(sxClick()), SLOT(nextPreset()));
	connect(this, SIGNAL(dxClick()), SLOT(prevPreset()));
}

void PowerAmplifierPreset::fillPresetDesc()
{
	preset_desc.reserve(num_preset);
	preset_desc.append(tr("Normal"));
	preset_desc.append(tr("Dance"));
	preset_desc.append(tr("Pop"));
	preset_desc.append(tr("Rock"));
	preset_desc.append(tr("Classical"));
	preset_desc.append(tr("Jazz"));
	preset_desc.append(tr("Party"));
	preset_desc.append(tr("Soft"));
	preset_desc.append(tr("Full Bass"));
	preset_desc.append(tr("Full Treble"));

	for (unsigned i=10; i < num_preset; ++i)
	{
		QString desc;
		desc.sprintf("%s %d", tr("Preset").ascii(), i + 1);
		preset_desc.append(desc);
	}

	QDomNode n = getPowerAmplifierNode();
	QDomNode node = n.firstChild();
	while (!node.isNull())
	{
		QRegExp reg("pre\\d{1,2}");
		int pos = reg.search(node.nodeName());
		if (pos != -1)
		{
			int preset_id = node.nodeName().mid(pos + 3, reg.matchedLength()).toInt();
			if (preset_id > 0 && preset_id <= (int)num_preset)
				preset_desc[preset_id - 1] = node.toElement().text();
			else
				qWarning("[AUDIO] Preset %d is not a valid preset number", preset_id);
		}
		node = node.nextSibling();
	}
}

QDomNode PowerAmplifierPreset::getPowerAmplifierNode()
{
	QDomNode node_page = getPageNode(DIFSON_MULTI);
	if (!node_page.isNull())
	{
		QDomNode node_item = getChildWithId(node_page, QRegExp("item\\d{1,2}"), AMBIENTE);
		if (!node_item.isNull())
			return getChildWithId(node_item, QRegExp("device\\d{1,2}"), POWER_AMPLIFIER);
	}

	node_page = getPageNode(DIFSON);
	if (!node_page.isNull())
		return getChildWithId(node_page, QRegExp("item\\d{1,2}"), POWER_AMPLIFIER);
	return QDomNode();
}

void PowerAmplifierPreset::showEvent(QShowEvent *event)
{
	SetTextU(preset_desc[preset].ascii());
	Draw();
}

void PowerAmplifierPreset::prevPreset()
{
	qDebug("PowerAmplifierPreset::prevPreset()");
	if (!preset)
		preset = num_preset - 1;
	else
		--preset;
	SetTextU(preset_desc[preset].ascii());
	Draw();
}

void PowerAmplifierPreset::nextPreset()
{
	qDebug("PowerAmplifierPreset::nextPreset()");
	if (preset + 1 >= num_preset)
		preset = 0;
	else
		++preset;
	SetTextU(preset_desc[preset].ascii());
	Draw();
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

/*****************************************************************
 ** PowerAmplifierBalance
 ****************************************************************/

PowerAmplifierBalance::PowerAmplifierBalance(QWidget *parent, const char *name, QColor SecondForeground)
 : BannOnOffCombo(parent, name)
{
	qDebug("PowerAmplifierBalance::PowerAmplifierBalance()");
	SetIcons(IMG_PLUS, IMG_MINUS, IMG_BALANCE, IMG_BALANCE_SX, IMG_BALANCE_DX);
	secondary_fg = SecondForeground;
	balance = 0;
	connect(this, SIGNAL(sxClick()), SLOT(dx()));
	connect(this, SIGNAL(dxClick()), SLOT(sx()));
}

void PowerAmplifierBalance::sx()
{
	qDebug("PowerAmplifierBalance::sx()");
	if (balance >= 0 && balance - 10 <= 0)
		changeStatus(balance - 10 ? SX : CENTER);

	balance -= 10;
	showBalance();
	Draw();
}

void PowerAmplifierBalance::dx()
{
	qDebug("PowerAmplifierBalance::dx()");
	if (balance <= 0 && balance + 10 >= 0)
		changeStatus(balance + 10 ? DX : CENTER);

	balance += 10;
	showBalance();
	Draw();
}

void PowerAmplifierBalance::showBalance()
{
	QString desc;
	desc.sprintf("%d", abs(balance));
	SetSecondaryTextU(desc.ascii());
}

void PowerAmplifierBalance::setFGColor(QColor c)
{
	banner::setFGColor(c);
	SecondaryText->setPaletteForegroundColor(secondary_fg);
}

/*****************************************************************
 ** PowerAmplifierLoud
 ****************************************************************/

PowerAmplifierLoud::PowerAmplifierLoud(QWidget *parent, const char *name)
 : bannOnOff(parent, name)
{
	qDebug("PowerAmplifierLoud::PowerAmplifierLoud()");
	SetIcons(ICON_ON, ICON_OFF, IMG_LOUD_ON, IMG_LOUD_OFF);
	impostaAttivo(0);
	connect(this, SIGNAL(sxClick()), SLOT(loudOn()));
	connect(this, SIGNAL(dxClick()), SLOT(loudOff()));
}

void PowerAmplifierLoud::loudOff()
{
	qDebug("PowerAmplifierLoud::loudOff()");
	impostaAttivo(0);
	Draw();
}

void PowerAmplifierLoud::loudOn()
{
	qDebug("PowerAmplifierLoud::loudOn()");
	impostaAttivo(1);
	Draw();
}
