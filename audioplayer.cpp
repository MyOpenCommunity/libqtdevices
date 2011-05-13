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
#include "multimedia.h"
#include "labels.h" // ScrollingLabel
#include "xmldevice.h"

#include <QFontMetrics>
#include <QGridLayout>
#include <QLabel>
#include <QVariant> // for setProperty

// The timeout for a single item in msec
#define LOOP_TIMEOUT 2000

// Max characters displayed together in the title
#define MAX_TITLE_CHARS 32


QVector<AudioPlayerPage *> AudioPlayerPage::audioplayer_pages(MAX_MEDIA_TYPE, 0);

BtButton *AudioPlayerPage::tray_icon = 0;

namespace
{
	// strips the decimal dot from the time returned by mplayer; if length is passed,
	// the result is left-padded with "00:" until length
	QString formatTime(const QString &mp_time, int length = 0)
	{
		QString res = mp_time;
		int dot = mp_time.indexOf('.');

		// strip decimal point
		if (dot > 0)
			res = mp_time.left(dot);

		// left-pad with "00:"
		if (length > 0)
			while (res.length() < length)
				res = "00:" + res;

		return res;
	}
}


UPnpListManager::UPnpListManager(XmlDevice *d)
{
	dev = d;
	connect(dev, SIGNAL(responseReceived(XmlResponse)), SLOT(handleResponse(XmlResponse)));
	connect(dev, SIGNAL(error(int,int)), SLOT(handleError(int,int)));
}

QString UPnpListManager::currentFilePath()
{
	Q_ASSERT_X(!current_file.isNull(), "UPnpListManager::currentFilePath", "Called with current_file not initialized!");
	return current_file.path;
}

void UPnpListManager::handleResponse(const XmlResponse &response)
{
	if (response.contains(XmlResponses::TRACK_SELECTION))
	{
		current_file = response[XmlResponses::TRACK_SELECTION].value<EntryInfo>();
		emit currentFileChanged();
	}
}

void UPnpListManager::handleError(int response, int code)
{
	// NOTE: See the comment on managing errors in UPnpClientBrowser::handleError
	if ((response == XmlResponses::TRACK_SELECTION || response == XmlResponses::INVALID) &&
			code == XmlError::SERVER_DOWN)
		emit serverDown();
}

void UPnpListManager::nextFile()
{
	if (++index >= total_files)
		index = 0;
	dev->nextFile();
}

void UPnpListManager::previousFile()
{
	if (--index < 0)
		index = total_files - 1;
	dev->previousFile();
}

int UPnpListManager::currentIndex()
{
	return index;
}

int UPnpListManager::totalFiles()
{
	return total_files;
}

void UPnpListManager::setCurrentIndex(int i)
{
	index = i;
}

void UPnpListManager::setTotalFiles(int n)
{
	total_files = n;
}

void UPnpListManager::setStartingFile(EntryInfo starting_file)
{
	dev->selectFile(starting_file.name);
	current_file = starting_file;
}

EntryInfo::Metadata UPnpListManager::currentMeta()
{
	return current_file.metadata;
}


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
	if (type == AudioPlayerPage::UPNP_FILE)
	{
		UPnpListManager* upnp = new UPnpListManager(bt_global::xml_device);
		connect(upnp, SIGNAL(serverDown()), SLOT(handleServerDown()));
		list_manager = upnp;
	}
	else
		list_manager = new FileListManager;

	connect(list_manager, SIGNAL(currentFileChanged()), SLOT(currentFileChanged()));

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


	QFont title_font = bt_global::font->get(FontManager::PLAYER_TITLE);
	QFontMetrics fm(title_font);
	description_top = new ScrollingLabel;
	description_top->setFont(title_font);
	description_top->setMaximumWidth(fm.averageCharWidth() * MAX_TITLE_CHARS);

	description_bottom = new ScrollingLabel;
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
	l_btn->setContentsMargins(2, 0, 2, 0);
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
	const char *done_slot = (type == IP_RADIO) ? SLOT(quit()) : SLOT(mplayerDone());
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
	l->setContentsMargins(10, 0, 25, 0);
	l->addWidget(bg, 1, Qt::AlignCenter);
	l->addLayout(l_btn, 1);
	if (volume)
		l->addWidget(volume, 1, Qt::AlignCenter);
	else
		l->addStretch(1);

	connect(player, SIGNAL(playingInfoUpdated(QMap<QString,QString>)), SLOT(refreshPlayInfo(QMap<QString,QString>)));
	connect(&refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

#ifdef LAYOUT_TS_10
	// create the tray icon and add it to tray
	if (!tray_icon)
	{
		tray_icon = new BtButton(bt_global::skin->getImage("tray_player"));
		bt_global::btmain->trayBar()->addButton(tray_icon, TrayBar::AUDIO_PLAYER);
		tray_icon->hide();
	}
#endif

	connect(player, SIGNAL(mplayerStarted()), SLOT(playerStarted()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(refreshPlayInfo()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(playerStopped()));
}

int AudioPlayerPage::sectionId() const
{
	return MULTIMEDIA;
}

void AudioPlayerPage::startMPlayer(QString filename, int time)
{
	clearLabels();

	if (type == IP_RADIO)
	{
		description_top->setText(tr("Loading..."));
		description_bottom->setText(tr("Loading..."));
	}

	player->play(filename, true);
	refresh_data.start(MPLAYER_POLLING);
}

void AudioPlayerPage::startPlayback()
{
	int index = list_manager->currentIndex();
	int total_files = list_manager->totalFiles();
	track->setText(tr("Track: %1 / %2").arg(index + 1).arg(total_files));
	startMPlayer(list_manager->currentFilePath(), 0);
}

void AudioPlayerPage::clearLabels()
{
	description_top->setText(" ");
	description_bottom->setText(" ");
	elapsed->setText(" ");
}

void AudioPlayerPage::playAudioFilesBackground(QList<QString> files, unsigned element)
{
	Q_ASSERT_X(type != AudioPlayerPage::UPNP_FILE, "AudioPlayerPage::playAudioFilesBackground",
		"The function must not be called with type UPNP_FILE!");
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

void AudioPlayerPage::playAudioFile(EntryInfo starting_file, int file_index, int num_files)
{
	Q_ASSERT_X(type == AudioPlayerPage::UPNP_FILE, "AudioPlayerPage::playAudioFile",
		"The function must be called with type UPNP_FILE!");

	UPnpListManager *um = static_cast<UPnpListManager*>(list_manager);
	um->setStartingFile(starting_file);
	um->setCurrentIndex(file_index);
	um->setTotalFiles(num_files);

	showPage();
	loop_starting_file = -1;
	qDebug() << "AudioPlayerPage::playAudioFile";

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
	FileListManager *lm = static_cast<FileListManager*>(list_manager);

	lm->setList(entries);
	lm->setCurrentIndex(element);

	showPage();
	qDebug() << "AudioPlayerPage::playAudioFiles";
	loop_starting_file = -1;

	startPlayback();
}

void AudioPlayerPage::resetLoopCheck()
{
	// avoid the loop-detection code kicking in
	qDebug() << "AudioPlayerPage::resetLoopCheck";
	loop_starting_file = -1;
}

void AudioPlayerPage::handleServerDown()
{
	playerStopped();

	if (isVisible())
		emit Closed();
	else
		emit serverDown();
}

void AudioPlayerPage::quit()
{
	stop();
	playerStopped();
}

void AudioPlayerPage::currentFileChanged()
{
	clearLabels();
	if (player->isPaused())
		player->requestInitialPlayingInfo(list_manager->currentFilePath());
	else
		startPlayback();

	int index = list_manager->currentIndex();
	int total_files = list_manager->totalFiles();
	track->setText(tr("Track: %1 / %2").arg(index + 1).arg(total_files));
}

void AudioPlayerPage::mplayerDone()
{
	if (loop_starting_file == -1)
	{
		qDebug() << "AudioPlayerPage::mplayerDone loop starting file:" << list_manager->currentIndex();
		loop_starting_file = list_manager->currentIndex();
		loop_total_time = list_manager->totalFiles() * LOOP_TIMEOUT;

		loop_time_counter.start();
	}
	else if (loop_starting_file == list_manager->currentIndex())
	{
		qDebug() << "AudioPlayerPage::mplayerDone loop elapsed:" << loop_time_counter.elapsed() << "total time:" << loop_total_time;
		if (loop_time_counter.elapsed() < loop_total_time)
		{
			qWarning() << "MediaPlayer: loop detected, force stop";
			player->stop();
			playerStopped();
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

void AudioPlayerPage::refreshPlayInfo(const QMap<QString, QString> &attrs)
{
	if (type == LOCAL_FILE || type == UPNP_FILE)
	{
		EntryInfo::Metadata md = list_manager->currentMeta();

		if (attrs.contains("meta_title"))
			description_top->setText(attrs["meta_title"]);
		else if (md.contains("title") && !md["title"].isEmpty())
			description_top->setText(md["title"]);
		else if (attrs.contains("file_name"))
			description_top->setText(attrs["file_name"]);

		if (attrs.contains("meta_artist"))
			description_bottom->setText(attrs["meta_artist"]);
		else if (md.contains("artist") && !md["artist"].isEmpty())
			description_bottom->setText(md["artist"]);
		else if (attrs.contains("meta_album"))
			description_bottom->setText(attrs["meta_album"]);
		else if (md.contains("album") && !md["album"].isEmpty())
			description_bottom->setText(md["album"]);

		QString total;
		// mplayer sometimes shows a wrong duration: we give the precedence to
		// the duration from upnp if present.
		if (md.contains("total_time") && !md["total_time"].isEmpty())
			total = formatTime(md["total_time"]);
		else if (attrs.contains("total_time"))
			total = formatTime(attrs["total_time"]);

		QString current;
		if (attrs.contains("current_time"))
			current = formatTime(attrs["current_time"], total.length());
		else if (attrs.contains("current_time_only"))
			current = formatTime(attrs["current_time_only"], total.length());

		if (!total.isEmpty() && !current.isEmpty())
			elapsed->setText(current + " / " + total);
	}
	else if (type == IP_RADIO)
	{
		if (attrs.contains("stream_url"))
		{
			stream_url_exist = true;
			description_top->setText(attrs["stream_url"]);
		}
		else if (attrs.contains("file_name"))
		{
			filename_exist = true;
			description_top->setText(attrs["file_name"]);
		}
		if (!stream_url_exist && !filename_exist)
			description_top->setText(tr("Information not available"));

		if (attrs.contains("stream_title"))
		{
			stream_title_exist = true;
			description_bottom->setText(attrs["stream_title"]);
		}
		if (!stream_title_exist)
			description_bottom->setText(tr("Information not available"));
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
	filename_exist = false;
	MultimediaSectionPage::current_player = this;
#ifdef LAYOUT_TS_10
	tray_icon->setVisible(true);
#endif
}

void AudioPlayerPage::playerStopped()
{
	// handle the next-while-paused case
	if (isPlayerPaused())
		return;

	if (MultimediaSectionPage::current_player == this)
	{
		emit playerExited();
		MultimediaSectionPage::current_player = 0;
	}
#ifdef LAYOUT_TS_10
	tray_icon->setVisible(false);
#endif
}
