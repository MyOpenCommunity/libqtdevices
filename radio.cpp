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
#include "displaycontrol.h" //  bt_global::display
#include "mediaplayer.h" // bt_global::sound
#include "audiostatemachine.h" // bt_global::audio_states
#include "main.h" // SOUND_PATH
#include "hardware_functions.h" // beep

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QFile>
#include <QLCDNumber>


#define REQUEST_FREQUENCY_TIME 1000
#define MEMORY_PRESS_TIME 3000

int RadioPage::save_sound_delay = 900;

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


RadioInfo::RadioInfo(QString _area, RadioSourceDevice *_dev)
{
	screensaver_running = false;
	shown = false;
	area = _area;
	dev = _dev;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	radio_name = new QLabel;
	radio_name->setFont(bt_global::font->get(FontManager::RADIO_NAME));

	channel = new QLabel;
	channel->setFont(bt_global::font->get(FontManager::RADIO_MEMORY_NUMBER));

#ifdef LAYOUT_TS_3_5
	frequency = new QLCDNumber;
	frequency->setSegmentStyle(QLCDNumber::Flat);
	frequency->setSmallDecimalPoint(true);
	frequency->setNumDigits(6);
	frequency->setLineWidth(0);

	BtButton *cycle = new BtButton(bt_global::skin->getImage("cycle_tracks"));
	connect(cycle, SIGNAL(clicked()), SIGNAL(nextStation()));

	// The LCDNumber has a primitive API that does not support text of different
	// sizes. The only way to obtain a big LCDnumber is to force a specific
	// width; unfortunately to obtain it we have to use a width of 190 pixels but
	// the screen size is only 240, 230 exluding margins, so we have to overlap
	// the LCDNumber with the other widgets (channel and cycle).

	int freq_width = 190;
	// the 10 below are for the margins of the page: 5 on the left, 5 on the right
	int freq_x = maxWidth() - freq_width - 10;
	frequency->setParent(this);
	frequency->setGeometry(freq_x, 0, freq_width, cycle->height());

	QGridLayout *grid = new QGridLayout(this);
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setHorizontalSpacing(10);
	grid->setVerticalSpacing(0);
	grid->addWidget(cycle, 0, 0);
	grid->addWidget(channel, 0, 1);
	grid->addItem(new QSpacerItem(10, 10), 0, 2);
	grid->addWidget(radio_name, 1, 0, 1, 3, Qt::AlignHCenter);
	grid->setColumnStretch(2, 1);

#else
	frequency = new QLabel;
	frequency->setFont(bt_global::font->get(FontManager::RADIO_STATION));

	QGridLayout *grid = new QGridLayout(this);
	grid->setContentsMargins(10, 0, 5, 0);
	grid->setSpacing(0);
	grid->addWidget(radio_name, 0, 0, 1, 1, Qt::AlignCenter);
	grid->addWidget(channel, 1, 0, 1, 1, Qt::AlignRight);
	grid->addWidget(frequency, 1, 0, 1, 1, Qt::AlignLeft);
#endif

	setFrequency(-1);
	setChannel(-1);
	setRadioName(QString());

	connect(bt_global::display, SIGNAL(startscreensaver(Page*)), SLOT(screensaverStarted()));
	connect(bt_global::display, SIGNAL(stopscreensaver()), SLOT(screensaverStopped()));
}

#ifdef LAYOUT_TS_10
void RadioInfo::setBackgroundImage(const QString &background_image)
{
	QPixmap background = *bt_global::icons_cache.getIcon(background_image);

	setMaximumSize(background.size());
	setPixmap(background);
}
#endif

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
	QString freq_text;
	if (freq < 0)
		freq_text = ("-.--");
	else
		freq_text = QString::number(freq / 100.0, 'f', 2);

#ifdef LAYOUT_TS_3_5
	frequency->display(freq_text);
#else
	frequency->setText(QString(tr("FM %1").arg(freq_text));
#endif
}

void RadioInfo::setChannel(int memory_channel)
{
	QString text;
	if (memory_channel < 0)
	{
#ifdef LAYOUT_TS_3_5
		text = "--:";
#else
		text = QString(tr("Channel: %1")).arg("-");
#endif
	}
	else
	{
#ifdef LAYOUT_TS_3_5
		text = QString("%1:").arg(memory_channel);
#else
		text = QString(tr("Channel: %1")).arg(memory_channel);
#endif
	}

	channel->setText(text);
}

void RadioInfo::setRadioName(const QString &rds)
{
	if (rds.isEmpty())
		radio_name->setText("----");
	else
		radio_name->setText(rds);
}


RadioPage::RadioPage(RadioSourceDevice *_dev, const QString &amb)
{
	dev = _dev;
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

#ifdef LAYOUT_TS_3_5
	radio_info = new RadioInfo(QString(), dev);
	QWidget *main_widget = new QWidget;
	QGridLayout *main_layout = new QGridLayout(main_widget);
	main_layout->addWidget(radio_info, 0, 0, 1, 4);
	buildPage(main_widget, nav_bar);

	connect(radio_info, SIGNAL(nextStation()), SLOT(nextStation()));

#else
	buildPage(createContent(), nav_bar, amb);
#endif
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

#ifdef LAYOUT_TS_10
QWidget *RadioPage::createContent()
{
	QWidget *content = new QWidget;

	// radio description, with frequency and memory station
	radio_info = new RadioInfo(QString(), dev);
	radio_info->setBackgroundImage(bt_global::skin->getImage("details_display"));

	// tuning control, manual/auto buttons
	minus_button = new BtButton(bt_global::skin->getImage("minus"));
	plus_button = new BtButton(bt_global::skin->getImage("plus"));

	manual_off = bt_global::skin->getImage("man_off");
	manual_on = bt_global::skin->getImage("man_on");
	auto_off = bt_global::skin->getImage("auto_off");
	auto_on = bt_global::skin->getImage("auto_on");

	auto_button = new BtButton(auto_on);
	manual_button = new BtButton(manual_off);

	connect(auto_button, SIGNAL(clicked()), SLOT(setAuto()));
	connect(manual_button, SIGNAL(clicked()), SLOT(setManual()));
	connect(minus_button, SIGNAL(clicked()), SLOT(frequencyDown()));
	connect(plus_button, SIGNAL(clicked()), SLOT(frequencyUp()));

	BtButton *next_station = new BtButton(bt_global::skin->getImage("next"));
	BtButton *prev_station = new BtButton(bt_global::skin->getImage("previous"));
	connect(next_station, SIGNAL(clicked()), SLOT(nextStation()));
	connect(prev_station, SIGNAL(clicked()), SLOT(previousStation()));

	QGridLayout *tuning = new QGridLayout;
	tuning->setContentsMargins(0, 0, 19, 0);
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
		BtButton *b = new BtButton(bt_global::skin->getImage(QString("num_%1").arg(i + 1)));
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
#endif

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
		connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(enterBeepState(int)));
		bt_global::audio_states->toState(AudioStates::BEEP_ON);
	}
	else if (state == AudioStates::BEEP_ON)
		QTimer::singleShot(save_sound_delay, this, SLOT(playSaveSound()));
#endif
#if defined(BT_HARDWARE_TS_3_5)
	beep();
#endif
}

void RadioPage::enterBeepState(int new_state)
{
	// avoid problems in case of state-change races
	if (new_state != AudioStates::BEEP_ON)
		return;

	disconnect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), this, SLOT(enterBeepState(int)));
	connect(bt_global::sound, SIGNAL(soundFinished()), SLOT(exitBeepState()));
	QTimer::singleShot(save_sound_delay, this, SLOT(playSaveSound()));
}

void RadioPage::playSaveSound()
{
	bt_global::sound->play(SOUND_PATH "beep.wav");
}

void RadioPage::exitBeepState()
{
	bt_global::audio_states->removeState(AudioStates::BEEP_ON);
	disconnect(bt_global::sound, SIGNAL(soundFinished()), this, SLOT(exitBeepState()));
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

