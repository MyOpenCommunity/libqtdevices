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


#include "radio.h"
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "navigation_bar.h" // NavigationBar
#include "media_device.h" // RadioSourceDevice
#include "icondispatcher.h" // bt_global::icons_cache
#include "btmain.h" // startscreensaver, stopscreensaver
#include "mediaplayer.h" // bt_global::sound
#include "audiostatemachine.h" // bt_global::audio_states
#include "main.h" // SOUND_PATH

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QFile>


namespace
{

	QWidget *composeButtons(BtButton *left, BtButton *right)
	{
		QWidget *w = new QWidget;
		QHBoxLayout *hbox = new QHBoxLayout(w);
		hbox->setContentsMargins(0, 0, 0, 0);
		hbox->setSpacing(10);
		hbox->addWidget(left);
		hbox->addWidget(right);
		return w;
	}
}


RadioInfo::RadioInfo(const QString &background_image, QString _area, RadioSourceDevice *_dev)
{
	screensaver_running = false;
	shown = false;
	area = _area;
	dev = _dev;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	QPixmap background = *bt_global::icons_cache.getIcon(background_image);

	setMaximumSize(background.size());
	setPixmap(background);

	QGridLayout *grid = new QGridLayout(this);
	grid->setContentsMargins(10, 0, 5, 0);
	grid->setSpacing(0);

	radio_name = new QLabel;
	radio_name->setFont(bt_global::font->get(FontManager::RADIO_NAME));
	grid->addWidget(radio_name, 0, 0, 1, 1, Qt::AlignCenter);

	channel = new QLabel;
	channel->setFont(bt_global::font->get(FontManager::RADIO_MEMORY_NUMBER));
	grid->addWidget(channel, 1, 0, 1, 1, Qt::AlignRight);

	frequency = new QLabel;
	frequency->setFont(bt_global::font->get(FontManager::RADIO_STATION));
	grid->addWidget(frequency, 1, 0, 1, 1, Qt::AlignLeft);

	setFrequency(-1);
	setChannel(-1);
	setRadioName(QString());

	connect(bt_global::btmain, SIGNAL(startscreensaver(Page*)), SLOT(screensaverStarted()));
	connect(bt_global::btmain, SIGNAL(stopscreensaver()), SLOT(screensaverStopped()));
}

void RadioInfo::setArea(const QString &_area)
{
	area = _area;
}

void RadioInfo::screensaverStarted()
{
	screensaver_running = true;
	if (shown && dev->rdsUpdates())
		dev->requestStopRDS();
}

void RadioInfo::screensaverStopped()
{
	screensaver_running = false;
	if (shown && dev->isActive(area) && !dev->rdsUpdates())
		dev->requestStartRDS();
}

void RadioInfo::valueReceived(const DeviceValues &values_list)
{
	foreach (int dim, values_list.keys())
	{
		switch (dim)
		{
		case RadioSourceDevice::DIM_AREAS_UPDATED:
			if (shown && !screensaver_running)
			{
				if (dev->isActive(area) && !dev->rdsUpdates())
					dev->requestStartRDS();
				else if (!dev->isActive(area) && dev->rdsUpdates())
					dev->requestStopRDS();
			}
			break;
		case RadioSourceDevice::DIM_RDS:
			setRadioName(values_list[dim].toString());
			break;
		case RadioSourceDevice::DIM_FREQUENCY:
			setFrequency(values_list[dim].toInt());
			break;
		case RadioSourceDevice::DIM_TRACK:
			setChannel(values_list[dim].toInt());
			break;
		}
	}
}

void RadioInfo::isShown(bool sh)
{
	shown = sh;
	// we won't activate/disactivate the RDS updates when the screensaver is
	// running, even if something change the current page.
	if (dev->isActive(area) && !screensaver_running)
	{
		if (shown)
			dev->requestStartRDS();
		else
			dev->requestStopRDS();
	}
}

void RadioInfo::setFrequency(const int freq)
{
	if (freq < 0)
		frequency->setText(QString(tr("FM %1")).arg("-.--"));
	else
		frequency->setText(QString(tr("FM %1")).arg(freq / 100.0, 0, 'f', 2));
}

void RadioInfo::setChannel(int memory_channel)
{
	if (memory_channel < 0)
		channel->setText(QString(tr("Channel: %1")).arg("-"));
	else
		channel->setText(QString(tr("Channel: %1")).arg(memory_channel));
}

void RadioInfo::setRadioName(const QString &rds)
{
	if (rds.isEmpty())
		radio_name->setText("----");
	else
		radio_name->setText(rds);
}


#define REQUEST_FREQUENCY_TIME 1000
#define MEMORY_PRESS_TIME 3000

RadioPage::RadioPage(RadioSourceDevice *_dev, const QString &amb)
{
	dev = _dev;
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	buildPage(createContent(), nav_bar, amb);
	memory_number = 0;
	memory_timer.setInterval(MEMORY_PRESS_TIME);
	memory_timer.setSingleShot(true);
	connect(&memory_timer, SIGNAL(timeout()), SLOT(storeMemoryStation()));

	request_frequency.setInterval(REQUEST_FREQUENCY_TIME);
	request_frequency.setSingleShot(true);
	connect(&request_frequency, SIGNAL(timeout()), SLOT(requestFrequency()));
}

void RadioPage::setArea(const QString &area)
{
	radio_info->setArea(area);
}

void RadioPage::hideEvent(QHideEvent *)
{
	radio_info->isShown(false);
}

void RadioPage::showEvent(QShowEvent *)
{
	radio_info->isShown(true);
}

QWidget *RadioPage::createContent()
{
	QWidget *content = new QWidget;

	// radio description, with frequency and memory station
	radio_info = new RadioInfo(bt_global::skin->getImage("details_display"), QString(), dev);

	// tuning control, manual/auto buttons
	minus_button = new BtButton(bt_global::skin->getImage("minus"));
	plus_button = new BtButton(bt_global::skin->getImage("plus"));
	auto_button = new BtButton;
	manual_button = new BtButton;

	manual_off = bt_global::skin->getImage("man_off");
	manual_on = bt_global::skin->getImage("man_on");
	auto_off = bt_global::skin->getImage("auto_off");
	auto_on = bt_global::skin->getImage("auto_on");
	manual_button->setImage(manual_off);
	auto_button->setImage(auto_on);

	connect(auto_button, SIGNAL(clicked()), SLOT(setAuto()));
	connect(manual_button, SIGNAL(clicked()), SLOT(setManual()));
	connect(minus_button, SIGNAL(clicked()), SLOT(frequencyDown()));
	connect(plus_button, SIGNAL(clicked()), SLOT(frequencyUp()));

	BtButton *next_station = new BtButton(bt_global::skin->getImage("next"));
	BtButton *prev_station = new BtButton(bt_global::skin->getImage("previous"));
	connect(next_station, SIGNAL(clicked()), SLOT(nextStation()));
	connect(prev_station, SIGNAL(clicked()), SLOT(previousStation()));

	QGridLayout *tuning = new QGridLayout;
	tuning->setContentsMargins(0, 0, 0, 0);
	tuning->setSpacing(0);
	tuning->addWidget(composeButtons(minus_button, plus_button), 0, 0);
	tuning->setColumnStretch(1, 1);
	tuning->addWidget(composeButtons(prev_station, next_station), 0, 2);
	tuning->setColumnStretch(3, 1);
	tuning->addWidget(composeButtons(auto_button, manual_button), 0, 4);

	// memory numbers buttons
	QList<BtButton *> buttons;
	QHBoxLayout *memory = new QHBoxLayout;
	memory->setContentsMargins(0, 0, 0, 0);
	memory->setSpacing(10);
	memory->addStretch(1);
	for (int i = 0; i < 5; ++i)
	{
		BtButton *b = new BtButton;
		QString str = QString("num_%1").arg(i + 1);
		b->setImage(bt_global::skin->getImage(str));
		buttons << b;
		// Each button must be associated with the memory location, which go 1-5 instead of 0-4
		button_group.addButton(b, i + 1);
		memory->addWidget(b);
	}
	// below it's not right. If the user presses the button for a long time, the station is saved.
	// On click, change memory station
	connect(&button_group, SIGNAL(buttonClicked(int)), SLOT(changeStation(int)));
	connect(&button_group, SIGNAL(buttonPressed(int)), SLOT(memoryButtonPressed(int)));
	connect(&button_group, SIGNAL(buttonReleased(int)), SLOT(memoryButtonReleased(int)));
	memory->addStretch(1);

	// add everything to layout
	QVBoxLayout *main = new QVBoxLayout(content);
	main->setContentsMargins(10, 0, 10, 45);
	main->setSpacing(0);
	main->addWidget(radio_info);
	main->addLayout(tuning);
	main->addLayout(memory);

	manual = false;

	return content;
}

void RadioPage::changeStation(int station_num)
{
	qDebug("Changing to station number: %d", station_num);
	dev->setStation(QString::number(station_num));
}

void RadioPage::memoryButtonPressed(int but_num)
{
	memory_timer.start();
	memory_number = but_num;
	qDebug("Memory button pressed: %d", but_num);
}

void RadioPage::memoryButtonReleased(int but_num)
{
	memory_timer.stop();
	qDebug("Memory button released: %d", but_num);
}

void RadioPage::storeMemoryStation()
{
	qDebug("Storing frequency to memory station %d", memory_number);
	dev->saveStation(QString::number(memory_number));

#if defined(BT_HARDWARE_TS_10) || defined(BT_HARDWARE_X11)
	int state = bt_global::audio_states->currentState();

	if (!QFile::exists(SOUND_PATH "beep.wav"))
		return;

	// when in idle state, we need first to switch to a state where the local amplifier is on,
	// and then remove it from the stack; when in beep state we can play the sound right away;
	// do not try to play the beep when in different states
	if (state == AudioStates::IDLE)
	{
		connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(playSaveSound(int)));
		bt_global::audio_states->toState(AudioStates::BEEP_ON);
	}
	else if (state == AudioStates::BEEP_ON)
		bt_global::sound->play(SOUND_PATH "beep.wav");
#endif
#if defined(BT_HARDWARE_TS_3_5)
	beep();
#endif
}

void RadioPage::playSaveSound(int new_state)
{
	// avoid problems in case of state-change races
	if (new_state != AudioStates::BEEP_ON)
		return;

	disconnect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), this, SLOT(playSaveSound(int)));
	connect(bt_global::sound, SIGNAL(soundFinished()), SLOT(saveSoundFinished()));
	bt_global::sound->play(SOUND_PATH "beep.wav");
}

void RadioPage::saveSoundFinished()
{
	bt_global::audio_states->removeState(AudioStates::BEEP_ON);
	disconnect(bt_global::sound, SIGNAL(soundFinished()), this, SLOT(saveSoundFinished()));
}

void RadioPage::setAuto()
{
	if (!manual)
		return;

	manual = false;

	plus_button->setAutoRepeat(false);
	minus_button->setAutoRepeat(false);
	manual_button->setImage(manual_off);
	auto_button->setImage(auto_on);
}

void RadioPage::setManual()
{
	if (manual)
		return;

	manual = true;

	plus_button->setAutoRepeat(true);
	minus_button->setAutoRepeat(true);
	manual_button->setImage(manual_on);
	auto_button->setImage(auto_off);
}

void RadioPage::nextStation()
{
	qDebug("Selecting next station");
	dev->nextTrack();
}

void RadioPage::previousStation()
{
	qDebug("Selecting previous station");
	dev->prevTrack();
}

void RadioPage::requestFrequency()
{
	dev->requestFrequency();
}

void RadioPage::frequencyUp()
{
	if (manual)
	{
		dev->frequenceUp("1");
		request_frequency.start();
	}
	else
		dev->frequenceUp();
}

void RadioPage::frequencyDown()
{
	if (manual)
	{
		dev->frequenceDown("1");
		request_frequency.start();
	}
	else
		dev->frequenceDown();
}

