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
#include "generic_functions.h" // scsToLocalVolume
#include "btbutton.h" // needed to directly connect button signals with slots
#include "media_device.h"

#include <QVariant> // setProperty
#include <QDomNode>
#include <QDebug>
#include <QLabel>



AdjustVolume::AdjustVolume(QWidget *parent) :
	BannLevel(parent)
{
	current_level = 2;
}

void AdjustVolume::initBanner(const QString &left, const QString &_center_on, const QString &_center_off,
	const QString &right, States init_state, int init_level, const QString &banner_text)
{
	BannLevel::initBanner(banner_text);

	left_button->setImage(left);
	right_button->setImage(right);

	center_on = _center_on;
	center_off = _center_off;

	setLevel(init_level);
	setState(init_state);
}

void AdjustVolume::updateIcons()
{
	QString icon;
	switch (current_state)
	{
	case ON:
		icon = center_on;
		break;
	case OFF:
		icon = center_off;
		break;
	}
	setCenterLeftIcon(getBostikName(icon, QString("sxl") + QString::number(current_level)));
	setCenterRightIcon(getBostikName(icon, QString("dxl") + QString::number(current_level)));
}

void AdjustVolume::setState(States new_state)
{
	if (new_state != current_state)
	{
		current_state = new_state;
		updateIcons();
	}

}

void AdjustVolume::setLevel(int level)
{
	if (level != current_level)
	{
		current_level = level;
		updateIcons();
	}
}



BannPowerAmplifier::BannPowerAmplifier(const QString &descr, PowerAmplifierDevice *d, PowerAmplifierPage* page)
	: AdjustVolume(0)
{
	status = false;
	dev = d;

	on_icon = bt_global::skin->getImage("on");
	off_icon = bt_global::skin->getImage("off");

	initBanner(on_icon, bt_global::skin->getImage("volume_active"),
		bt_global::skin->getImage("volume_inactive"), bt_global::skin->getImage("settings"), OFF, 1, descr);

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	connect(left_button, SIGNAL(clicked()), SLOT(toggleStatus()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(volumeUp()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(volumeDown()));

	connectButtonToPage(right_button, page);
}

void BannPowerAmplifier::toggleStatus()
{
	if (status)
		dev->turnOff();
	else
		dev->turnOn();
}

void BannPowerAmplifier::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		if (it.key() == PowerAmplifierDevice::DIM_STATUS)
		{
			status = it.value().toBool();
			setState(status ? ON : OFF);
			left_button->setImage(status ? off_icon : on_icon);
		}
		else if (it.key() == PowerAmplifierDevice::DIM_VOLUME)
		{
			int volume = it.value().toInt();
			// We have to normalize the volume value (from 0 to 31) in a value
			// that we can represent into the banner (that accept values from 0 to 8)
			// so we use the following formula.
			int level = scsToLocalVolume(volume);
			// TODO remove after aligning image names
#ifdef LAYOUT_BTOUCH
			Q_ASSERT_X(level > 0, "BannPowerAmplifier::valueReceived", "Received volume is not in range 0-31");
#else
			Q_ASSERT_X(level >= 0, "BannPowerAmplifier::valueReceived", "Received volume is not in range 0-31");
#endif
			setLevel(level);
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


PowerAmplifierPage::PowerAmplifierPage(PowerAmplifierDevice *dev, const QDomNode &config_node)
{
	buildPage();
	loadBanners(dev, config_node);
}

void PowerAmplifierPage::loadBanners(PowerAmplifierDevice *dev, const QDomNode &config_node)
{
	QMap<int, QString> preset_list;
	foreach (const QDomNode &preset_node, getChildren(config_node, "pre"))
	{
		if (getTextChild(preset_node, "enable") == "1")
			preset_list[preset_node.nodeName().mid(3).toInt()] = getTextChild(preset_node, "descr");
	}

	banner *b = new PowerAmplifierPreset(dev, this, preset_list);
	page_content->appendBanner(b);

	b = new PowerAmplifierTreble(dev, tr("Treble"), this);
	page_content->appendBanner(b);

	b = new PowerAmplifierBass(dev, tr("Bass"), this);
	page_content->appendBanner(b);

	b = new PowerAmplifierBalance(dev, tr("Balance"));
	page_content->appendBanner(b);

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
	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));

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

void PowerAmplifierPreset::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
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
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("treble"), QString(),
		bt_global::skin->getImage("plus"), ON, banner_text, "");

	connect(left_button, SIGNAL(clicked()), SLOT(down()));
	connect(right_button, SIGNAL(clicked()), SLOT(up()));
	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));
	showLevel(0);
}

void PowerAmplifierTreble::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
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
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("bass"), QString(),
		bt_global::skin->getImage("plus"), ON, banner_text, "");

	connect(left_button, SIGNAL(clicked()), SLOT(down()));
	connect(right_button, SIGNAL(clicked()), SLOT(up()));
	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));
	showLevel(0);
}

void PowerAmplifierBass::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
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


PowerAmplifierBalance::PowerAmplifierBalance(PowerAmplifierDevice *d, const QString &descr) :
	BannOnOffCombo(0)
{
	dev = d;
	initBanner(bt_global::skin->getImage("less"), bt_global::skin->getImage("balance_dx"), bt_global::skin->getImage("balance"),
		bt_global::skin->getImage("balance_sx"), bt_global::skin->getImage("more"), CENTER, descr);
	connect(right_button, SIGNAL(clicked()), SLOT(dx()));
	connect(left_button, SIGNAL(clicked()), SLOT(sx()));
	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));
	showBalance(0);
}

void PowerAmplifierBalance::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
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

	setInternalText(QString::number(qAbs(balance)));
}


PowerAmplifierLoud::PowerAmplifierLoud(PowerAmplifierDevice *d, const QString &banner_text, QWidget *parent) :
	BannOnOffState(parent)
{
	dev = d;
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("loud"),
		bt_global::skin->getImage("on"), OFF, banner_text);
	connect(right_button, SIGNAL(clicked()), SLOT(on()));
	connect(left_button, SIGNAL(clicked()), SLOT(off()));
	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));
}

void PowerAmplifierLoud::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
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
