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
#include "mediaplayer.h"
#include "list_manager.h"
#include "btbutton.h"
#include "state_button.h"
#include "skinmanager.h"
#include "navigation_bar.h"
#include "sounddiffusionpage.h" // SoundDiffusionPage::showCurrentAmbientPage()
#include "fontmanager.h"

#include <QLabel>
#include <QTimer>
#include <QBoxLayout>


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

	temporary_pause = false;
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

	QLabel *title_label = new QLabel;
	title_label->setFont(bt_global::font->get(FontManager::PLAYER_TITLE));

	main_layout->addWidget(title_label, 0, Qt::AlignHCenter);
	main_layout->addSpacing(50);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->setSpacing(10);
	buttons_layout->setContentsMargins(0, 0, 0, 0);

	play_button = new StateButton;
	play_button->setOnImage(bt_global::skin->getImage("pause"));
	play_button->setOffImage(bt_global::skin->getImage("play"));
	play_button->setCheckable(true);
	connect(play_button, SIGNAL(clicked(bool)), SLOT(playToggled(bool)));

	BtButton *stop = new BtButton(bt_global::skin->getImage("stop"));
	connect(stop, SIGNAL(clicked()), SLOT(stop()));

	BtButton *prev = new BtButton(bt_global::skin->getImage("prev"));
	connect(prev, SIGNAL(clicked()), SLOT(prev()));

	BtButton *next = new BtButton(bt_global::skin->getImage("next"));
	connect(next, SIGNAL(clicked()), SLOT(next()));

	buttons_layout->addWidget(play_button);
	buttons_layout->addWidget(stop);
	buttons_layout->addWidget(prev);
	buttons_layout->addWidget(next);
	main_layout->addLayout(buttons_layout);
	main_layout->addSpacing(110);

	NavigationBar *nav_bar = new NavigationBar("goto_sounddiffusion");
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(gotoSoundDiffusion()));
	buildPage(content, nav_bar);

	// update the icon of the play button
	connect(player, SIGNAL(mplayerStarted()), SLOT(started()));
	connect(player, SIGNAL(mplayerResumed()), SLOT(started()));
	connect(player, SIGNAL(mplayerDone()), SLOT(stopped()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(stopped()));
	connect(player, SIGNAL(mplayerPaused()), SLOT(stopped()));
}

void AudioPlayerPage::started()
{
	play_button->setStatus(true);
	play_button->setChecked(true);
}

void AudioPlayerPage::stopped()
{
	play_button->setStatus(false);
	play_button->setChecked(false);
}

void AudioPlayerPage::playToggled(bool playing)
{
	if (playing)
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
	showPage();
}

void AudioPlayerPage::startPlayback()
{
	int index = list_manager->currentIndex();
	int total_files = list_manager->totalFiles();
//	track->setText(tr("Track: %1 / %2").arg(index + 1).arg(total_files));
	startMPlayer(list_manager->currentFilePath(), 0);
}

void AudioPlayerPage::startMPlayer(QString filename, int time)
{
//	clearLabels();

//	if (type == IP_RADIO)
//	{
//		description_top->setText(tr("Loading..."));
//		description_bottom->setText(tr("Loading..."));
//	}

	player->play(filename, true);
	refresh_data->start(MPLAYER_POLLING);
}

