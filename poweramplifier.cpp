/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "poweramplifier.h"
#include "xml_functions.h" // getChildWithId, getChildren
#include "devices_cache.h" // bt_global::devices_cache
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "generic_functions.h" // int trasformaVol(int vol)
#include "bannercontent.h"
#include "btbutton.h" // needed to directly connect button signals with slots

#include <QVariant> // setProperty
#include <QDomNode>
#include <QDebug>
#include <QLabel>


BannPowerAmplifier::BannPowerAmplifier(QWidget *parent, const QDomNode& config_node, QString address)
	: bannRegolaz(parent)
{
	setRange(1, 9);
	setValue(1);
	SkinContext context(getTextChild(config_node, "cid").toInt());
	SetIcons(bt_global::skin->getImage("settings"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("volume_active"), bt_global::skin->getImage("volume_inactive"), true);
	setAddress(address);
	dev = bt_global::add_device_to_cache(new PowerAmplifierDevice(address));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));

	connect(this, SIGNAL(dxClick()), SLOT(toggleStatus()));
	connect(this, SIGNAL(cdxClick()), SLOT(volumeUp()));
	connect(this, SIGNAL(csxClick()), SLOT(volumeDown()));

	status = false;

	connectDxButton(new PowerAmplifier(dev, config_node));
}

void BannPowerAmplifier::toggleStatus()
{
	if (status)
		dev->turnOff();
	else
		dev->turnOn();
}

void BannPowerAmplifier::inizializza(bool forza)
{
	dev->requestStatus();
	dev->requestVolume();
	banner::inizializza(forza);
}

void BannPowerAmplifier::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_STATUS)
		{
			status = it.value().toBool();
			impostaAttivo(status);
			SetIcons(1, bt_global::skin->getImage(status ? "off" : "on"));
			Draw();
		}
		else if (it.key() == PowerAmplifierDevice::DIM_VOLUME)
		{
			int volume = it.value().toInt();
			// We have to normalize the volume value (from 0 to 31) in a value
			// that we can represent into the banner (that accept values from 1 to 9)
			// so we use the following formula.
			setValue(trasformaVol(volume));
			Draw();
		}
		++it;
	}
}

void BannPowerAmplifier::volumeUp()
{
	dev->volumeUp();
}

void BannPowerAmplifier::volumeDown()
{
	dev->volumeDown();
}


PowerAmplifier::PowerAmplifier(PowerAmplifierDevice *dev, const QDomNode &config_node)
{
	buildPage();
	loadBanners(dev, config_node);
}

void PowerAmplifier::loadBanners(PowerAmplifierDevice *dev, const QDomNode &config_node)
{
	QMap<int, QString> preset_list;
	foreach (const QDomNode &preset_node, getChildren(config_node, "pre"))
		preset_list[preset_node.nodeName().mid(3).toInt()] = preset_node.toElement().text();

	banner *b = new PowerAmplifierPreset(dev, this, preset_list);
	b->Draw();
	page_content->appendBanner(b);

	b = new PowerAmplifierTreble(dev, tr("Treble"), this);
	page_content->appendBanner(b);

	b = new PowerAmplifierBass(dev, tr("Bass"), this);
	page_content->appendBanner(b);

	PowerAmplifierBalance *bann = new PowerAmplifierBalance(dev, this);
	bann->setText(tr("Balance"));
	bann->Draw();
	page_content->appendBanner(bann);

	b = new PowerAmplifierLoud(dev, tr("Loud"), this);
	page_content->appendBanner(b);
}


PowerAmplifierPreset::PowerAmplifierPreset(PowerAmplifierDevice *d, QWidget *parent, const QMap<int, QString>& preset_list)
	: Bann2Buttons(parent)
{
	dev = d;

	num_preset = 20;
	fillPresetDesc(preset_list);
	connect(right_button, SIGNAL(clicked()), SLOT(next()));
	connect(left_button, SIGNAL(clicked()), SLOT(prev()));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));

	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("preset"),
		bt_global::skin->getImage("plus"), preset_desc[0]);
}

void PowerAmplifierPreset::fillPresetDesc(const QMap<int, QString>& preset_list)
{
	preset_desc.reserve(num_preset);
	preset_desc.append(tr("Normal"));
	preset_desc.append(tr("Dance"));
	preset_desc.append(tr("Pop"));
	preset_desc.append(tr("Rock"));
	preset_desc.append(tr("Classical"));
	preset_desc.append(tr("Techno"));
	preset_desc.append(tr("Party"));
	preset_desc.append(tr("Soft"));
	preset_desc.append(tr("Full Bass"));
	preset_desc.append(tr("Full Treble"));

	for (int i = preset_desc.size(); i < num_preset; ++i)
		preset_desc.append(QString("%1 %2").arg(tr("User")).arg(i + 1 - 10));

	QMapIterator<int, QString> it(preset_list);
	while (it.hasNext())
	{
		it.next();
		if (it.key() > 0 && it.key() <= num_preset)
			preset_desc[it.key() - 1] = it.value();
	}
}

void PowerAmplifierPreset::inizializza(bool forza)
{
	dev->requestPreset();
	banner::inizializza(forza);
}

void PowerAmplifierPreset::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_PRESET)
		{
			int preset = it.value().toInt();
			if (preset >= 0 && preset < num_preset)
				setDescriptionText(preset_desc[preset]);
			else
				qWarning("Preset value (%d) is out of admitted range! [0 - %d]", preset, num_preset);
		}
		++it;
	}
}

void PowerAmplifierPreset::prev()
{
	dev->prevPreset();
}

void PowerAmplifierPreset::next()
{
	dev->nextPreset();
}


PowerAmplifierTreble::PowerAmplifierTreble(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent) :
	BannOnOff2Labels(parent)
{
	dev = d;
	setCentralTextSecondaryColor(true);
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("treble"),
		bt_global::skin->getImage("plus"), ON, banner_text, "");

	connect(left_button, SIGNAL(clicked()), SLOT(down()));
	connect(right_button, SIGNAL(clicked()), SLOT(up()));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));
	showLevel(0);
}

void PowerAmplifierTreble::inizializza(bool forza)
{
	dev->requestTreble();
	banner::inizializza(forza);
}

void PowerAmplifierTreble::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_TREBLE)
			showLevel(it.value().toInt());
		++it;
	}
}

void PowerAmplifierTreble::up()
{
	dev->trebleUp();
}

void PowerAmplifierTreble::down()
{
	dev->trebleDown();
}

void PowerAmplifierTreble::showLevel(int level)
{
	QString desc;
	desc.sprintf("%s%d", level > 0 ? "+" : "", level);
	setCentralText(desc);
}


PowerAmplifierBass::PowerAmplifierBass(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent) :
	BannOnOff2Labels(parent)
{
	dev = d;
	setCentralTextSecondaryColor(true);
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("bass"),
		bt_global::skin->getImage("plus"), ON, banner_text, "");

	connect(left_button, SIGNAL(clicked()), SLOT(down()));
	connect(right_button, SIGNAL(clicked()), SLOT(up()));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));
	showLevel(0);
}

void PowerAmplifierBass::inizializza(bool forza)
{
	dev->requestBass();
	banner::inizializza(forza);
}

void PowerAmplifierBass::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_BASS)
			showLevel(it.value().toInt());
		++it;
	}
}

void PowerAmplifierBass::up()
{
	dev->bassUp();
}

void PowerAmplifierBass::down()
{
	dev->bassDown();
}

void PowerAmplifierBass::showLevel(int level)
{
	QString desc;
	desc.sprintf("%s%d", level > 0 ? "+" : "", level);
	setCentralText(desc);
}


PowerAmplifierBalance::PowerAmplifierBalance(PowerAmplifierDevice *d, QWidget *parent) : BannOnOffCombo(parent)
{
	dev = d;
	SecondaryText->setProperty("SecondFgColor", true);
	SetIcons(bt_global::skin->getImage("more"), bt_global::skin->getImage("less"), bt_global::skin->getImage("balance"),
		bt_global::skin->getImage("balance_dx"), bt_global::skin->getImage("balance_sx"));
	connect(this, SIGNAL(sxClick()), SLOT(dx()));
	connect(this, SIGNAL(dxClick()), SLOT(sx()));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));
	showBalance(0);
}

void PowerAmplifierBalance::inizializza(bool forza)
{
	dev->requestBalance();
	banner::inizializza(forza);
}

void PowerAmplifierBalance::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_BALANCE)
			showBalance(it.value().toInt());
		++it;
	}
}

void PowerAmplifierBalance::sx()
{
	dev->balanceUp();
}

void PowerAmplifierBalance::dx()
{
	dev->balanceDown();
}

void PowerAmplifierBalance::showBalance(int balance)
{
	if (balance == 0)
		changeStatus(CENTER);
	else if (balance < 0)
		changeStatus(DX);
	else
		changeStatus(SX);

	QString desc;
	desc.sprintf("%d", abs(balance));
	setSecondaryText(desc);
	Draw();
}


PowerAmplifierLoud::PowerAmplifierLoud(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent) :
	BannOnOffState(parent)
{
	dev = d;
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("loud"),
		bt_global::skin->getImage("on"), OFF, banner_text);
	connect(this, SIGNAL(sxClick()), SLOT(on()));
	connect(this, SIGNAL(dxClick()), SLOT(off()));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));
}

void PowerAmplifierLoud::inizializza(bool forza)
{
	dev->requestLoud();
	banner::inizializza(forza);
}

void PowerAmplifierLoud::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_LOUD)
		{
			setState(it.value().toBool() ? ON : OFF);
		}
		++it;
	}
}

void PowerAmplifierLoud::off()
{
	dev->loudOff();
}

void PowerAmplifierLoud::on()
{
	dev->loudOn();
}
