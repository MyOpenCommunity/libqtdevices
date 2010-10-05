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
#include "pagestack.h" // bt_global::page_stack
#include "multimedia.h"

#include <QGridLayout>
#include <QLabel>
#include <QVariant> // for setProperty

// The timeout for a single item in msec
#define LOOP_TIMEOUT 2000

QVector<AudioPlayerPage *> AudioPlayerPage::audioplayer_pages(MAX_MEDIA_TYPE, 0);

BtButton *AudioPlayerPage::tray_icon = 0;


AudioPlayerPage *AudioPlayerPage::getAudioPlayerPage(MediaType type)
{
	Q_ASSERT_X(type < MAX_MEDIA_TYPE, "AudioPlayerPage::getAudioPlayerPage", "invalid type");

	if (!audioplayer_pages[type])
		audioplayer_pages[type] = new AudioPlayerPage(type);

	return audioplayer_pages[type];
}

void AudioPlayerPage::showPrevButton(bool show)
{
	player_buttons->showPrevButton(show);
}

void AudioPlayerPage::showNextButton(bool show)
{
	player_buttons->showNextButton(show);
}

QVector<AudioPlayerPage *>AudioPlayerPage::audioPlayerPages()
{
	return audioplayer_pages;
}

AudioPlayerPage::AudioPlayerPage(MediaType t)
{
	type = t;
	// Sometimes it happens that mplayer can't reproduce a song or a web radio,
	// for example because the network is down. In this case the mplayer exits
	// immediately with the signal mplayerDone (== everything ok). Since the
	// UI starts reproducing the next item when receiving the mplayerDone signal,
	// this causes an infinite loop. To avoid that, we count the time elapsed to
	// reproduce the whole item list, and if it is under LOOP_TIMEOUT * num_of_items
	// we stop the reproduction.
	loop_starting_file = -1;

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

	player_buttons = new MultimediaPlayerButtons(type == IP_RADIO ? MultimediaPlayerButtons::IPRADIO_PAGE : MultimediaPlayerButtons::AUDIO_PAGE);
	connectMultimediaButtons(player_buttons);

	connect(player_buttons, SIGNAL(previous()), SLOT(resetLoopCheck()));
	connect(player_buttons, SIGNAL(next()), SLOT(resetLoopCheck()));

	// a radio channel is without an end. If mplayer has finished maybe there is an error.
	const char *done_slot = (type == IP_RADIO) ? SLOT(quit()) : SLOT(next());
	connect(player, SIGNAL(mplayerDone()), done_slot);

	l_btn->addWidget(player_buttons);
	l_btn->addStretch(0);
	if (goto_sounddiff)
		l_btn->addWidget(goto_sounddiff);

	ItemTuning *volume = NULL;

	// if the touch is a sound diffusion source do not display the volume control
	if (!bt_global::audio_states->isSource())
	{
		volume = new ItemTuning(tr("Volume"), bt_global::skin->getImage("volume"));
		connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
	}

	QVBoxLayout *l = new QVBoxLayout(content);
	l->addWidget(bg, 1, Qt::AlignCenter);
	l->addLayout(l_btn, 1);
	if (volume)
		l->addWidget(volume, 1, Qt::AlignCenter);
	else
		l->addStretch(1);

	connect(player, SIGNAL(playingInfoUpdated(QMap<QString,QString>)), SLOT(refreshPlayInfo(QMap<QString,QString>)));
	connect(&refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

	// create the tray icon and add it to tray
	if (!tray_icon)
	{
		tray_icon = new BtButton(bt_global::skin->getImage("tray_player"));
		bt_global::btmain->trayBar()->addButton(tray_icon, TrayBar::AUDIO_PLAYER);
		tray_icon->hide();
	}

	connect(player, SIGNAL(mplayerStarted()), SLOT(playerStarted()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(playerStopped()));
}

int AudioPlayerPage::sectionId() const
{
	return MULTIMEDIA;
}

QString AudioPlayerPage::currentFileName(int index) const
{
	return file_list[index];
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
}

void AudioPlayerPage::clearLabels()
{
	description_top->setText(" ");
	description_bottom->setText(" ");
	elapsed->setText(" ");
}

void AudioPlayerPage::playAudioFilesBackground(QList<QString> files, unsigned element)
{
	current_file = element;
	total_files = files.size();
	file_list = files;

	loop_starting_file = -1;

	displayMedia(current_file);
}

void AudioPlayerPage::playAudioFiles(QList<QString> files, unsigned element)
{
	current_file = element;
	total_files = files.size();
	file_list = files;
	showPage();

	loop_starting_file = -1;

	displayMedia(current_file);
}

void AudioPlayerPage::resetLoopCheck()
{
	// avoid the loop-detection code kicking in
	loop_starting_file = -1;
}

void AudioPlayerPage::previous()
{
	clearLabels();
	MediaPlayerPage::previous();
	if (player->isPaused())
		player->requestInitialPlayingInfo(currentFileName(current_file));
	else
		displayMedia(current_file);
	track->setText(tr("Track: %1 / %2").arg(current_file + 1).arg(total_files));
}

void AudioPlayerPage::quit()
{
	stop();
	playerStopped();
}

void AudioPlayerPage::next()
{
	if (loop_starting_file == -1)
	{
		loop_starting_file = current_file;
		loop_total_time = total_files * LOOP_TIMEOUT;
		loop_time_counter.start();
	}
	else if (loop_starting_file == current_file)
	{
		if (loop_time_counter.elapsed() < loop_total_time)
		{
			qWarning() << "MediaPlayer: loop detected, force stop";
			quit();
			return;
		}
		else
		{
			// we restart the time counter to find loop that happens when the player
			// is already started.
			loop_time_counter.start();
		}
	}

	clearLabels();
	MediaPlayerPage::next();
	if (player->isPaused())
		player->requestInitialPlayingInfo(currentFileName(current_file));
	else
		displayMedia(current_file);
	track->setText(tr("Track: %1 / %2").arg(current_file + 1).arg(total_files));
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

void AudioPlayerPage::refreshPlayInfo(const QMap<QString, QString> &attrs)
{
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
                 {
                         stream_url_exist = true;
			 description_top->setText(attrs["stream_url"]);
		 }
                 if (!stream_url_exist)
                         description_top->setText(tr("Information not available"));
		 
                 if (attrs.contains("stream_title"))
                 {
                         stream_title_exist = true;
			 description_bottom->setText(attrs["stream_title"]);
		}
                if (!stream_title_exist)
                         description_bottom->setText(tr("Information not available"));
	}

	if (type == LOCAL_FILE && attrs.contains("total_time") && attrs.contains("current_time"))
	{
		QString total = formatTime(attrs["total_time"]);
		QString current = formatTime(attrs["current_time"], total);

		elapsed->setText(current + " / " + total);
	}
}

void AudioPlayerPage::refreshPlayInfo()
{
	QMap<QString, QString> attrs = player->getPlayingInfo();

	refreshPlayInfo(attrs);
}

void AudioPlayerPage::changeVolume(int volume)
{
	bt_global::audio_states->setVolume(volume);
}

void AudioPlayerPage::gotoSoundDiffusion()
{
	SoundDiffusionPage::showCurrentAmbientPage();
}

void AudioPlayerPage::playerStarted()
{
        stream_url_exist = false;
	stream_title_exist = false;
	MultimediaSectionPage::current_player = this;
	tray_icon->setVisible(true);
}

void AudioPlayerPage::playerStopped()
{
	// handle the next-while-paused case
	if (isPlayerPaused())
		return;

	if (MultimediaSectionPage::current_player == this)
		MultimediaSectionPage::current_player = 0;
	tray_icon->setVisible(false);
}
