#include "poweramplifier.h"
#include "xml_functions.h" // getChildWithId, getChildren
#include "devices_cache.h" // bt_global::devices_cache

#include <QVariant> // setProperty
#include <QDomNode>
#include <QRegExp>
#include <QString>
#include <QLabel>
#include <QList>


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


BannPowerAmplifier::BannPowerAmplifier(QWidget *parent, const QDomNode& config_node, QString indirizzo, QString onIcon,
	QString offIcon, QString onAmpl, QString offAmpl, QString settingIcon) : bannRegolaz(parent)
{
	setRange(1,9);
	setValue(1);
	SetIcons(settingIcon, offIcon, onAmpl, offAmpl, true);
	setAddress(indirizzo);
	dev = static_cast<poweramplifier_device*>(bt_global::devices_cache.get_poweramplifier_device(getAddress()));
	connect(dev, SIGNAL(status_changed(QMap<status_key_t, stat_var>)),
			SLOT(status_changed(QMap<status_key_t, stat_var>)));

	connect(this, SIGNAL(dxClick()), SLOT(toggleStatus()));
	connect(this, SIGNAL(cdxClick()), SLOT(turnUp()));
	connect(this, SIGNAL(csxClick()), SLOT(turnDown()));

	off_icon = offIcon;
	on_icon = onIcon;
	status = false;

	connectDxButton(new PowerAmplifier(config_node));
}

void BannPowerAmplifier::toggleStatus()
{
	SetIcons(1, status ? off_icon : on_icon);
	status = !status;
	Draw();
}

void BannPowerAmplifier::turnUp()
{
	qDebug("PowerAmplifier::turnUp()");
}

void BannPowerAmplifier::turnDown()
{
	qDebug("PowerAmplifier::turnDown()");
}

void BannPowerAmplifier::status_changed(QMap<poweramplifier_device::status_key_t, stat_var> st)
{

}


PowerAmplifier::PowerAmplifier(const QDomNode &config_node)
{
	loadBanners(config_node);
}

void PowerAmplifier::loadBanners(const QDomNode &config_node)
{
	QMap<int, QString> preset_list;
	foreach (const QDomNode &preset_node, getChildren(config_node, "pre"))
		preset_list[preset_node.nodeName().mid(3).toInt()] = preset_node.toElement().text();

	appendBanner(new PowerAmplifierPreset(this, preset_list));

	banner *b = new PowerAmplifierTreble(this);
	b->setText(tr("Treble"));
	appendBanner(b);

	b = new PowerAmplifierBass(this);
	b->setText(tr("Bass"));
	appendBanner(b);

	b = new PowerAmplifierBalance(this);
	b->setText(tr("Balance"));
	appendBanner(b);

	b = new PowerAmplifierLoud(this);
	b->setText(tr("Loud"));
	appendBanner(b);
}


PowerAmplifierPreset::PowerAmplifierPreset(QWidget *parent, const QMap<int, QString>& preset_list)
	: bannOnOff(parent)
{
	SetIcons(IMG_PLUS, IMG_MINUS, QString(), IMG_PRESET);
	curr_preset = 0;
	num_preset = 20;
	fillPresetDesc(preset_list);
	setText(preset_desc[curr_preset]);
	Draw();
	connect(this, SIGNAL(sxClick()), SLOT(nextPreset()));
	connect(this, SIGNAL(dxClick()), SLOT(prevPreset()));
}

void PowerAmplifierPreset::fillPresetDesc(const QMap<int, QString>& preset_list)
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

	for (unsigned i = preset_desc.size(); i < num_preset; ++i)
		preset_desc.append(QString("%1 %2").arg(tr("Preset")).arg(i + 1));

	QMapIterator<int, QString> it(preset_list);
	while (it.hasNext())
	{
		it.next();
		if (it.key() > 0 && it.key() <= num_preset)
			preset_desc[it.key() - 1] = it.value();
	}
}

void PowerAmplifierPreset::prevPreset()
{
	if (!curr_preset)
		curr_preset = num_preset - 1;
	else
		--curr_preset;
	setText(preset_desc[curr_preset]);
	Draw();
}

void PowerAmplifierPreset::nextPreset()
{
	if (curr_preset + 1 >= num_preset)
		curr_preset = 0;
	else
		++curr_preset;
	setText(preset_desc[curr_preset]);
	Draw();
}

/*****************************************************************
 ** PowerAmplifierTreble
 ****************************************************************/

PowerAmplifierTreble::PowerAmplifierTreble(QWidget *parent) : bannOnOff2scr(parent)
{
	SecondaryText->setProperty("SecondFgColor", true);
	SetIcons(IMG_MINUS, IMG_PLUS, QString(), IMG_TREBLE);
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
	SetIcons(IMG_MINUS, IMG_PLUS, QString(), IMG_BASS);
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
