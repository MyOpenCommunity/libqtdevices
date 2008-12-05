#include "poweramplifier.h"
#include "xml_functions.h" // getChildWithId
#include "device_cache.h" // btouch_device_cache
#include "sottomenu.h"

#include <QVariant> // setProperty
#include <QRegExp>
#include <QLabel>
#include <QList>

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
static const char *IMG_MORE = IMG_PATH "more.png";
static const char *IMG_LESS = IMG_PATH "less.png";


/*****************************************************************
 ** PowerAmplifier
 ****************************************************************/

PowerAmplifier::PowerAmplifier(QWidget *parent, QString indirizzo, QString onIcon, QString offIcon, QString onAmpl, QString offAmpl, QString settingIcon)
	: bannRegolaz(parent)
{
	qDebug("PowerAmplifier::PowerAmplifier()");
	setRange(1,9);
	setValue(1);
	SetIcons(settingIcon, offIcon ,onAmpl, offAmpl, true);
	setAddress(indirizzo);
	dev = static_cast<poweramplifier_device*>(btouch_device_cache.get_poweramplifier_device(getAddress()));
	connect(dev, SIGNAL(status_changed(QMap<status_key_t, stat_var>)),
			SLOT(status_changed(QMap<status_key_t, stat_var>)));

	connect(this, SIGNAL(sxClick()), SLOT(showSettings()));
	connect(this, SIGNAL(dxClick()), SLOT(toggleStatus()));
	connect(this, SIGNAL(cdxClick()), SLOT(turnUp()));
	connect(this, SIGNAL(csxClick()), SLOT(turnDown()));

	off_icon = offIcon;
	on_icon = onIcon;
	status = false;
	settings_page = new sottoMenu(NULL);

	banner *b = new PowerAmplifierPreset(settings_page);
	b->setText(tr("Preset"));
	settings_page->appendBanner(b);

	b = new PowerAmplifierTreble(settings_page);
	b->setText(tr("Treble"));
	settings_page->appendBanner(b);

	b = new PowerAmplifierBass(settings_page);
	b->setText(tr("Bass"));
	settings_page->appendBanner(b);

	b = new PowerAmplifierBalance(settings_page);
	b->setText(tr("Balance"));
	settings_page->appendBanner(b);

	b = new PowerAmplifierLoud(settings_page);
	b->setText(tr("Loud"));
	settings_page->appendBanner(b);

	settings_page->hide();
	connect(settings_page, SIGNAL(Closed()), settings_page, SLOT(hide()));
}

void PowerAmplifier::showSettings()
{
	settings_page->show();
}

void PowerAmplifier::toggleStatus()
{
	SetIcons(1, status ? off_icon : on_icon);
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

void PowerAmplifier::status_changed(QMap<poweramplifier_device::status_key_t, stat_var> st)
{

}

/*****************************************************************
 ** PowerAmplifierPreset
 ****************************************************************/

PowerAmplifierPreset::PowerAmplifierPreset(QWidget *parent) : bannOnOff(parent)
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
		QString desc = QString("%1 %2").arg( tr("Preset")).arg(i + 1);
		preset_desc.append(desc);
	}

	// TODO: rimuovere lettura xml da qua! Deve essere letto solo da pagine principali!
	QDomNode n = getPowerAmplifierNode();
	QDomNode node = n.firstChild();
	while (!node.isNull())
	{
		QRegExp reg("pre\\d{1,2}");
		int pos = reg.indexIn(node.nodeName());
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
	setText(preset_desc[preset]);
	Draw();
}

void PowerAmplifierPreset::prevPreset()
{
	qDebug("PowerAmplifierPreset::prevPreset()");
	if (!preset)
		preset = num_preset - 1;
	else
		--preset;
	setText(preset_desc[preset]);
	Draw();
}

void PowerAmplifierPreset::nextPreset()
{
	qDebug("PowerAmplifierPreset::nextPreset()");
	if (preset + 1 >= num_preset)
		preset = 0;
	else
		++preset;
	setText(preset_desc[preset]);
	Draw();
}

/*****************************************************************
 ** PowerAmplifierTreble
 ****************************************************************/

PowerAmplifierTreble::PowerAmplifierTreble(QWidget *parent) : bannOnOff2scr(parent)
{
	SecondaryText->setProperty("SecondFgColor", true);
	SetIcons(IMG_MINUS, IMG_PLUS, NULL, IMG_TREBLE);
	level = 0;
	qDebug("PowerAmplifierTreble::PowerAmplifierTreble()");
	connect(this, SIGNAL(sxClick()), SLOT(down()));
	connect(this, SIGNAL(dxClick()), SLOT(up()));
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
	setSecondaryText(desc);
}


/*****************************************************************
 ** PowerAmplifierBass
 ****************************************************************/

PowerAmplifierBass::PowerAmplifierBass(QWidget *parent) : bannOnOff2scr(parent)
{
	SecondaryText->setProperty("SecondFgColor", true);
	SetIcons(IMG_MINUS, IMG_PLUS, NULL, IMG_BASS);
	level = 0;
	qDebug("PowerAmplifierBass::PowerAmplifierTreble()");
	connect(this, SIGNAL(sxClick()), SLOT(down()));
	connect(this, SIGNAL(dxClick()), SLOT(up()));
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
	setSecondaryText(desc);
}


/*****************************************************************
 ** PowerAmplifierBalance
 ****************************************************************/

PowerAmplifierBalance::PowerAmplifierBalance(QWidget *parent) : BannOnOffCombo(parent)
{
	qDebug("PowerAmplifierBalance::PowerAmplifierBalance()");
	SecondaryText->setProperty("SecondFgColor", true);
	SetIcons(IMG_MORE, IMG_LESS, IMG_BALANCE, IMG_BALANCE_SX, IMG_BALANCE_DX);
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
	setSecondaryText(desc);
}


/*****************************************************************
 ** PowerAmplifierLoud
 ****************************************************************/

PowerAmplifierLoud::PowerAmplifierLoud(QWidget *parent) : bannOnOff(parent)
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
