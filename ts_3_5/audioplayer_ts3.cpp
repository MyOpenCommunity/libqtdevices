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


#include "audioplayer_ts3.h"
#include "mediaplayer.h" // MediaPlayer, formatTime
#include "list_manager.h"
#include "btbutton.h"
#include "state_button.h"
#include "skinmanager.h"
#include "navigation_bar.h"
#include "sounddiffusionpage.h" // SoundDiffusionPage::showCurrentAmbientPage()
#include "fontmanager.h"
#include "labels.h" // ScrollingLabel
#include "pagestack.h"

#include <QLabel>
#include <QTimer>
#include <QBoxLayout>


// The timeout for a single item in msec
#define LOOP_TIMEOUT 2000


namespace
{
	ScrollingLabel *addInfoLabel(QGridLayout *layout, QString text)
	{
		QLabel *l = new QLabel(text);
		l->setFont(bt_global::font->get(FontManager::PLAYER_INFO));
		int row = layout->rowCount();
		layout->addWidget(l, row + 1, 0);

		ScrollingLabel *info_label = new ScrollingLabel;
		info_label->setFont(bt_global::font->get(FontManager::PLAYER_INFO));
		layout->addWidget(info_label, row + 1, 1);
		return info_label;
	}
}


QVector<AudioPlayerPage *> AudioPlayerPage::audioplayer_pages(MAX_MEDIA_TYPE, 0);


AudioPlayerPage *AudioPlayerPage::getAudioPlayerPage(MediaType type)
{
	if (!audioplayer_pages[type])
		audioplayer_pages[type] = new AudioPlayerPage(type);

	return audioplayer_pages[type];
}

QVector<AudioPlayerPage *>AudioPlayerPage::audioPlayerPages()
{
	return audioplayer_pages;
}

AudioPlayerPage::AudioPlayerPage(MediaType t)
{
	player = new MediaPlayer(this);
	refresh_data = new QTimer(this);
	connect(player, SIGNAL(playingInfoUpdated(QMap<QString,QString>)), SLOT(refreshPlayInfo(QMap<QString,QString>)));
	connect(refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

	connect(player, SIGNAL(mplayerStarted()), SLOT(started()));
	connect(player, SIGNAL(mplayerResumed()), SLOT(started()));
	connect(player, SIGNAL(mplayerDone()), SLOT(stopped()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(stopped()));
	connect(player, SIGNAL(mplayerPaused()), SLOT(stopped()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(refreshPlayInfo()));

	// a radio channel is without an end. If mplayer has finished maybe there is an error.
	const char *done_slot = (type == IP_RADIO) ? SLOT(stop()) : SLOT(mplayerDone());
	connect(player, SIGNAL(mplayerDone()), done_slot);

	list_manager = 0;

	type = t;
	popup_mode = false;
	if (type == AudioPlayerPage::UPNP_FILE)
	{
		UPnpListManager* upnp = new UPnpListManager(bt_global::xml_device);
		connect(upnp, SIGNAL(serverDown()), SLOT(handleServerDown()));
		list_manager = upnp;
	}
	else
		list_manager = new FileListManager;

	connect(list_manager, SIGNAL(currentFileChanged()), SLOT(currentFileChanged()));
	buildUi();
}

void AudioPlayerPage::buildUi()
{
	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(10);

	if (type == UPNP_FILE)
	{
		QGridLayout *info_layout = new QGridLayout;
		info_layout->setContentsMargins(0, 10, 0, 0);
		info_layout->setVerticalSpacing(20);
		info_layout->setHorizontalSpacing(5);
		track = addInfoLabel(info_layout, tr("Track:"));
		artist = addInfoLabel(info_layout, tr("Artist:"));
		album = addInfoLabel(info_layout, tr("Album:"));
		length = addInfoLabel(info_layout, tr("Length:"));
		main_layout->addLayout(info_layout);
	}
	else
	{
		radio_url = new ScrollingLabel;
		radio_url->setFont(bt_global::font->get(FontManager::PLAYER_RADIO_URL));
		main_layout->addWidget(radio_url, 0, Qt::AlignHCenter);
		main_layout->addSpacing(10);

		radio_title = new ScrollingLabel;
		radio_title->setFont(bt_global::font->get(FontManager::PLAYER_RADIO_TITLE));
		main_layout->addWidget(radio_title, 0, Qt::AlignHCenter);
		main_layout->addSpacing(35);
	}

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->setSpacing(10);
	buttons_layout->setContentsMargins(0, 0, 0, 0);

	play_button = new StateButton;
	play_button->setOnImage(bt_global::skin->getImage("pause"));
	play_button->setOffImage(bt_global::skin->getImage("play"));
	connect(play_button, SIGNAL(clicked()), SLOT(playToggled()));

	BtButton *stop = new BtButton(bt_global::skin->getImage("stop"));
	connect(stop, SIGNAL(clicked()), SLOT(stop()));

	BtButton *prev = new BtButton(bt_global::skin->getImage("prev"));
	connect(prev, SIGNAL(clicked()), SLOT(previous()));
	connect(prev, SIGNAL(clicked()), SLOT(resetLoopCheck()));

	BtButton *next = new BtButton(bt_global::skin->getImage("next"));
	connect(next, SIGNAL(clicked()), SLOT(next()));
	connect(next, SIGNAL(clicked()), SLOT(resetLoopCheck()));

	buttons_layout->addWidget(play_button);
	buttons_layout->addWidget(stop);
	buttons_layout->addWidget(prev);
	buttons_layout->addWidget(next);
	main_layout->addLayout(buttons_layout);
	main_layout->addStretch();

	NavigationBar *nav_bar = new NavigationBar("goto_sounddiffusion");
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(gotoSoundDiffusion()));
	buildPage(content, nav_bar);
}

void AudioPlayerPage::started()
{
	play_button->setStatus(StateButton::ON);
	refresh_data->start();
}

void AudioPlayerPage::stopped()
{
	play_button->setStatus(StateButton::OFF);
	refresh_data->stop();
}

void AudioPlayerPage::playToggled()
{
	if (play_button->getStatus() == StateButton::OFF)
		resume();
	else
		pause();
}

void AudioPlayerPage::stop()
{
	player->stop();
	emit Closed();
}

void AudioPlayerPage::pause()
{
	player->pause();
}

void AudioPlayerPage::resume()
{
	if (player->isInstanceRunning())
		player->resume();
	else
		startPlayback();
}

void AudioPlayerPage::previous()
{
	player->quit();
	list_manager->previousFile();
}

void AudioPlayerPage::next()
{
	player->quit();
	list_manager->nextFile();
}

void AudioPlayerPage::gotoSoundDiffusion()
{
	SoundDiffusionPage::showCurrentAmbientPage();
}

bool AudioPlayerPage::isPlayerInstanceRunning() const
{
	return player->isInstanceRunning();
}

bool AudioPlayerPage::isPlayerPaused() const
{
	return player->isPaused();
}

void AudioPlayerPage::playAudioFilesBackground(QList<QString> files, unsigned element)
{
	Q_ASSERT_X(type != AudioPlayerPage::UPNP_FILE, "AudioPlayerPage::playAudioFilesBackground",
		"The function must not be called with type UPNP_FILE!");
	popup_mode = true;
	FileListManager *lm = static_cast<FileListManager*>(list_manager);

	EntryInfoList entries;
	foreach (const QString &filename, files)
		entries << EntryInfo(filename, EntryInfo::AUDIO, filename);

	lm->setList(entries);
	lm->setCurrentIndex(element);

	loop_starting_file = -1;
	qDebug() << "AudioPlayerPage::playAudioFilesBackground";

	startPlayback();
}

void AudioPlayerPage::playAudioFiles(QList<QString> files, unsigned element)
{
	Q_ASSERT_X(type != AudioPlayerPage::UPNP_FILE, "AudioPlayerPage::playAudioFiles",
		"The function must not be called with type UPNP_FILE!");

	EntryInfoList entries;
	foreach (const QString &filename, files)
		entries << EntryInfo(filename, EntryInfo::AUDIO, filename);

	playAudioFiles(entries, element);
}

void AudioPlayerPage::playAudioFiles(EntryInfoList entries, unsigned element)
{
	popup_mode = false;
	FileListManager *lm = static_cast<FileListManager*>(list_manager);

	lm->setList(entries);
	lm->setCurrentIndex(element);

	showPage();
	qDebug() << "AudioPlayerPage::playAudioFiles";
	loop_starting_file = -1;

	startPlayback();
}

void AudioPlayerPage::playAudioFile(EntryInfo starting_file, int file_index, int num_files)
{
	Q_ASSERT_X(type == AudioPlayerPage::UPNP_FILE, "AudioPlayerPage::playAudioFile",
		"The function must be called with type UPNP_FILE!");

	popup_mode = false;
	UPnpListManager *um = static_cast<UPnpListManager*>(list_manager);
	um->setStartingFile(starting_file);
	um->setCurrentIndex(file_index);
	um->setTotalFiles(num_files);

	showPage();
	loop_starting_file = -1;
	qDebug() << "AudioPlayerPage::playAudioFile";

	startPlayback();
}

void AudioPlayerPage::showPage()
{
	// When we click the back button, the AudioPlayerPage must turn back
	// to the 'album' of the first 'song' played. This is possible when the
	// user navigates through the MultimediaFileListPage, which stores and saves
	// the 'playing context'. But when the reproduction starts from a spontaneous
	// event (i.e. the local source turned on) we can't save the context, so
	// the back should return to the previous page, using the page stack.
	if (popup_mode)
		bt_global::page_stack.showUserPage(this);
	Page::showPage();
}

void AudioPlayerPage::startPlayback()
{
	startMPlayer(list_manager->currentFilePath(), 0);
}

void AudioPlayerPage::startMPlayer(QString filename, int time)
{
	clearLabels();

	player->play(filename, true);
	refresh_data->start(MPLAYER_POLLING);
}

void AudioPlayerPage::clearLabels()
{
	if (type == UPNP_FILE)
	{
		track->setScrollingText("");
		album->setScrollingText("");
		artist->setScrollingText("");
		length->setScrollingText("");
	}
	else
	{
		radio_title_info = false;
		radio_url_info = false;
		radio_url->setScrollingText(tr("Loading..."));
		radio_title->setScrollingText(tr("Loading..."));
	}
}

void AudioPlayerPage::refreshPlayInfo(const QMap<QString, QString> &attrs)
{
	if (type == UPNP_FILE)
	{
		EntryInfo::Metadata md = list_manager->currentMeta();

		if (attrs.contains("meta_title"))
			track->setScrollingText(attrs["meta_title"]);
		else if (md.contains("title") && !md["title"].isEmpty())
			track->setScrollingText(md["title"]);

		if (attrs.contains("meta_artist"))
			artist->setScrollingText(attrs["meta_artist"]);
		else if (md.contains("artist") && !md["artist"].isEmpty())
			artist->setScrollingText(md["artist"]);

		if (attrs.contains("meta_album"))
			album->setScrollingText(attrs["meta_album"]);
		else if (md.contains("album") && !md["album"].isEmpty())
			album->setScrollingText(md["album"]);

		QString total;
		// mplayer sometimes shows a wrong duration: we give the precedence to
		// the duration from upnp if present.
		if (md.contains("total_time") && !md["total_time"].isEmpty())
			total = md["total_time"];
		else if (attrs.contains("total_time"))
			total = attrs["total_time"];

		QString current;
		if (attrs.contains("current_time"))
			current = attrs["current_time"];
		else if (attrs.contains("current_time_only"))
			current = attrs["current_time_only"];

		if (!total.isEmpty() && !current.isEmpty())
			length->setScrollingText(formatTime(current, total));

	}
	else if (type == IP_RADIO)
	{
		if (attrs.contains("stream_url"))
		{
			radio_url_info = true;
			radio_url->setScrollingText(attrs["stream_url"]);
		}
		else if (attrs.contains("file_name") && !radio_url_info)
		{
			radio_url_info = true;
			radio_url->setScrollingText(attrs["file_name"]);
		}
		if (!radio_url_info)
			radio_url->setScrollingText(tr("Information not available"));

		if (attrs.contains("stream_title"))
		{
			radio_title_info = true;
			radio_title->setScrollingText(attrs["stream_title"]);
		}
		if (!radio_title_info)
			radio_title->setScrollingText(tr("Information not available"));
	}
}

void AudioPlayerPage::refreshPlayInfo()
{
	QMap<QString, QString> attrs = player->getPlayingInfo();

	refreshPlayInfo(attrs);
}

void AudioPlayerPage::currentFileChanged()
{
	clearLabels();
	if (player->isPaused())
		player->requestInitialPlayingInfo(list_manager->currentFilePath());
	else
		startPlayback();
}

void AudioPlayerPage::mplayerDone()
{
	if (loop_starting_file == -1)
	{
		loop_starting_file = list_manager->currentIndex();
		loop_total_time = list_manager->totalFiles() * LOOP_TIMEOUT;

		loop_time_counter.start();
	}
	else if (loop_starting_file == list_manager->currentIndex())
	{
		if (loop_time_counter.elapsed() < loop_total_time)
		{
			qWarning() << "MediaPlayer: loop detected, force stop";
			player->stop();
			stopped();
			emit loopDetected();
			return;
		}
		else
		{
			// we restart the time counter to find loop that happens when the player
			// is already started.
			loop_time_counter.start();
		}
	}
	next();
}

void AudioPlayerPage::resetLoopCheck()
{
	// avoid the loop-detection code kicking in
	loop_starting_file = -1;
}

void AudioPlayerPage::handleServerDown()
{
	stopped();

	if (isVisible())
		emit Closed();
	else
		emit serverDown();
}
