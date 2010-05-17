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


#include "audioplayer.h"
#include "multimedia_buttons.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "items.h" // ItemTuning
#include "fontmanager.h"
#include "btbutton.h"
#include "mediaplayer.h"
#include "hardware_functions.h" // setVolum
#include "btmain.h"
#include "homewindow.h" // TrayBar
#include "sounddiffusionpage.h" // showCurrentAmbientPage
#include "main.h" // MULTIMEDIA
#include "media_device.h"
#include "devices_cache.h"
#include "audiostatemachine.h"

#include <QGridLayout>
#include <QLabel>
#include <QVariant> // for setProperty


AudioPlayerTray *AudioPlayerPage::tray_icon = NULL;


AudioPlayerTray::AudioPlayerTray(const QString &icon) :
	BtButton(icon)
{
	current_player = NULL;
	hide();

	connect(this, SIGNAL(clicked()), SLOT(gotoPlayer()));
}

void AudioPlayerTray::started()
{
	show();
	current_player = static_cast<AudioPlayerPage*>(sender());
}

void AudioPlayerTray::stopped()
{
	AudioPlayerPage *player = static_cast<AudioPlayerPage*>(sender());

	if (player != current_player)
		return;

	hide();
	current_player = NULL;
}

void AudioPlayerTray::gotoPlayer()
{
	if (current_player)
		current_player->showPage();
}


AudioPlayerPage::AudioPlayerPage(MediaType t)
{
	type = t;

	QWidget *content = new QWidget;
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);

	buildPage(content, nav_bar, QString(), TINY_TITLE_HEIGHT);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	QLabel *bg = new QLabel;
	QGridLayout *l_bg = new QGridLayout(bg);

	bg->setPixmap(bt_global::skin->getImage("audioplayer_background"));

	description_top = new QLabel;
	description_bottom = new QLabel;
	description_top->setFont(bt_global::font->get(FontManager::PLAYER_TITLE));
	description_bottom->setFont(bt_global::font->get(FontManager::PLAYER_AUTHOR));

	track = new QLabel;
	elapsed = new QLabel;
	track->setFont(bt_global::font->get(FontManager::PLAYER_INFO));
	elapsed->setFont(bt_global::font->get(FontManager::PLAYER_INFO));
	track->setProperty("PlayerInfo", true);
	elapsed->setProperty("PlayerInfo", true);

	l_bg->addWidget(description_top, 0, 0, 1, 2, Qt::AlignCenter);
	l_bg->addWidget(description_bottom, 1, 0, 1, 2, Qt::AlignCenter);
	l_bg->addWidget(track, 2, 0, Qt::AlignVCenter|Qt::AlignLeft);
	l_bg->addWidget(elapsed, 2, 1, Qt::AlignVCenter|Qt::AlignRight);

	QHBoxLayout *l_btn = new QHBoxLayout;
	BtButton *goto_sounddiff = NULL;
	if (bt_global::audio_states->isSource())
	{
		goto_sounddiff = new BtButton(bt_global::skin->getImage("goto_sounddiffusion"));
		connect(goto_sounddiff, SIGNAL(clicked()), SLOT(gotoSoundDiffusion()));
	}

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(type == IP_RADIO ? MultimediaPlayerButtons::IPRADIO_PAGE : MultimediaPlayerButtons::AUDIO_PAGE);
	connectMultimediaButtons(buttons);

	l_btn->addWidget(buttons);
	l_btn->addStretch(0);
	if (goto_sounddiff)
		l_btn->addWidget(goto_sounddiff);

	ItemTuning *volume = NULL;

	// if the touch is a sound diffusion source do not display the volume control
	if (!bt_global::audio_states->isSource())
	{
		dev = NULL;

		volume = new ItemTuning(tr("Volume"), bt_global::skin->getImage("volume"));
		connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
	}
	else
	{
		dev = bt_global::add_device_to_cache(new VirtualSourceDevice((*bt_global::config)[SOURCE_ADDRESS]));

		connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	}

	QVBoxLayout *l = new QVBoxLayout(content);
	l->addWidget(bg, 1, Qt::AlignCenter);
	l->addLayout(l_btn, 1);
	if (volume)
		l->addWidget(volume, 1, Qt::AlignCenter);
	else
		l->addStretch(1);

	connect(&refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

	// create the tray icon and add it to tray
	if (!tray_icon)
	{
		tray_icon = new AudioPlayerTray(bt_global::skin->getImage("tray_player"));
		bt_global::btmain->trayBar()->addButton(tray_icon);
	}

	connect(this, SIGNAL(started()), tray_icon, SLOT(started()));
	connect(this, SIGNAL(terminated()), tray_icon, SLOT(stopped()));
	connect(this, SIGNAL(started()), SLOT(playbackStarted()));
	connect(this, SIGNAL(stopped()), SLOT(playbackStopped()));
}

int AudioPlayerPage::sectionId() const
{
	return MULTIMEDIA;
}

void AudioPlayerPage::playbackStarted()
{
	if (!bt_global::audio_states->isSource())
		bt_global::audio_states->toState(AudioStates::PLAY_MEDIA_TO_SPEAKER);
}

void AudioPlayerPage::playbackStopped()
{
	if (!bt_global::audio_states->isSource())
		bt_global::audio_states->exitCurrentState();
}

void AudioPlayerPage::startMPlayer(int index, int time)
{
	if (type == IP_RADIO)
	{
		description_top->setText(tr("Loading..."));
		description_bottom->setText(tr("Loading..."));
	}
	else
	{
		description_top->setText(tr(" "));
		description_bottom->setText(tr(" "));
	}

	player->play(file_list[index], true);
	refresh_data.start(MPLAYER_POLLING);
}

void AudioPlayerPage::displayMedia(int index)
{
	track->setText(tr("Track: %1 / %2").arg(index + 1).arg(total_files));
	startMPlayer(index, 0);
	emit started();
}

void AudioPlayerPage::playAudioFiles(QList<QString> files, unsigned element)
{
	current_file = element;
	total_files = files.size();
	file_list = files;
	showPage();

	displayMedia(current_file);
}

// strips the decimal dot from the time returned by mplayer; if match_length is passed,
// the result is left-padded with "00:" to match match_length length
static QString formatTime(const QString &mp_time, const QString &match_length = QString())
{
	QString res = mp_time;
	int dot = mp_time.indexOf('.');

	// strip decimal point
	if (dot > 0)
		res = mp_time.left(dot);

	// left-pad with "00:"
	while (res.length() < match_length.length())
		res = "00:" + res;

	return res;
}

void AudioPlayerPage::refreshPlayInfo()
{
	QMap<QString, QString> attrs = player->getPlayingInfo();

	if (type == LOCAL_FILE)
	{
		if (attrs.contains("meta_title"))
			description_top->setText(attrs["meta_title"]);
		else if (attrs.contains("file_name"))
			description_top->setText(attrs["file_name"]);

		if (attrs.contains("meta_artist"))
			description_bottom->setText(attrs["meta_artist"]);
		else if (attrs.contains("meta_album"))
			description_bottom->setText(attrs["meta_album"]);
	}
	else if (type == IP_RADIO)
	{
		 if (attrs.contains("stream_url"))
			 description_top->setText(attrs["stream_url"]);

		 if (attrs.contains("stream_title"))
			 description_bottom->setText(attrs["stream_title"]);
	}

	if (type == LOCAL_FILE && attrs.contains("total_time") && attrs.contains("current_time"))
	{
		QString total = formatTime(attrs["total_time"]);
		QString current = formatTime(attrs["current_time"], total);

		elapsed->setText(current + " / " + total);
	}
}

void AudioPlayerPage::changeVolume(int volume)
{
	setVolume(VOLUME_MMDIFFUSION, volume);
}

void AudioPlayerPage::gotoSoundDiffusion()
{
	SoundDiffusionPage::showCurrentAmbientPage();
}

void AudioPlayerPage::valueReceived(const DeviceValues &device_values)
{
	// when we are turned off on all areas, pause the reproduction;
	// when we are turned back on, resume the reproduction
	if (device_values.contains(SourceDevice::DIM_AREAS_UPDATED))
	{
		bool status = dev->isActive();

		if (status)
		{
			if (!player->isInstanceRunning())
			{
				// TODO implement
				qDebug("Start playing a random mp3");
			}
			else if (player->isPaused())
				resume();
		}
		else if (player->isInstanceRunning())
			pause();
	}

	if (!player->isInstanceRunning())
		return;

	if (device_values.contains(VirtualSourceDevice::REQ_NEXT_TRACK))
		next();
	else if (device_values.contains(VirtualSourceDevice::REQ_PREV_TRACK))
		previous();
}
