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


#include "videoplayer.h"
#include "multimedia_buttons.h"
#include "navigation_bar.h"
#include "mediaplayer.h"
#include "displaycontrol.h" // forceOperativeMode
#include "items.h" // ItemTuning
#include "skinmanager.h"
#include "audiostatemachine.h"
#include "pagestack.h"
#include "fontmanager.h" // bt_global::font

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileInfo>

#include <QtDebug>

#define BUTTONS_TIMEOUT 5000


// VideoPlayerPage implementation

VideoPlayerPage::VideoPlayerPage()
{
	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);
	QHBoxLayout *v = new QHBoxLayout;

	// file name
	title = new QLabel;

	// only used for layout
	video = new QLabel;
	video->setFixedSize(320, 240);
	video->setStyleSheet("background: black");
	video->setFont(bt_global::font->get(FontManager::TEXT));
	video->setAlignment(Qt::AlignCenter);

	v->addStretch(1);
	v->addWidget(video);
	v->addStretch(1);

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::VIDEO_PAGE);
	connectMultimediaButtons(buttons);

	// handle mplayer termination
	connect(player, SIGNAL(mplayerDone()), SLOT(next()));

	l->addWidget(title, 0, Qt::AlignHCenter);
	l->addLayout(v, 1);
	l->addWidget(buttons, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar(QString(), QString(), QString(), "back");
	buildPage(content, nav_bar);

	window = new VideoPlayerWindow(this, player);

	// signals for navigation and to start/stop playback
	connect(buttons, SIGNAL(fullScreen()), SLOT(displayFullScreen()));

	connect(nav_bar, SIGNAL(backClick()), SLOT(stop()));

	// close the page when the user clicks the stop button on the
	// full screen playback window
	connect(window, SIGNAL(Closed()), SLOT(stop()));

	// disable/reenable screen saver
	connect(player, SIGNAL(mplayerStarted()), SLOT(videoPlaybackStarted()));
	connect(player, SIGNAL(mplayerResumed()), SLOT(videoPlaybackStarted()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(videoPlaybackStopped()));

	connect(player, SIGNAL(playingInfoUpdated(QMap<QString,QString>)), SLOT(refreshPlayInfo(QMap<QString,QString>)));
	connect(&refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));
}

VideoPlayerPage::~VideoPlayerPage()
{
	player->stop();
	delete player;
}

void VideoPlayerPage::showPage()
{
	MediaPlayerPage::showPage();
	fullscreen = false;
	resumePlayOnShow();
}

void VideoPlayerPage::startMPlayer()
{
	if (fullscreen)
		player->playVideoFullScreen(file_list[current_file], current_time);
	else
		player->playVideo(file_list[current_file], playbackGeometry(), current_time);
	refresh_data.start(MPLAYER_POLLING);
}

void VideoPlayerPage::displayMedia(int index)
{
	title->setText(QFileInfo(file_list[index]).fileName());
	current_file = index;
	current_time = 0;

	if (!player->checkVideoResolution(file_list[index]))
	{
		qWarning() << "VideoPlayerPage::displayMedia -> Video resolution too high";
		video->setText(tr("Video not supported"));
		return;
	}
	video->setText(QString());

	QTimer::singleShot(0, this, SLOT(startMPlayer()));
}

void VideoPlayerPage::displayVideos(QList<QString> videos, unsigned element)
{
	current_file = element;
	total_files = videos.size();
	file_list = videos;
	showPage();

	displayMedia(current_file);
}

void VideoPlayerPage::videoPlaybackTerminated()
{
	MediaPlayerPage::videoPlaybackTerminated();

	video->update();
}

void VideoPlayerPage::videoPlaybackStarted()
{
	bt_global::display->forceOperativeMode(true);
}

void VideoPlayerPage::videoPlaybackStopped()
{
	bt_global::display->forceOperativeMode(false);
}

void VideoPlayerPage::cleanUp()
{
	stop();
}

void VideoPlayerPage::aboutToHideEvent()
{
	MediaPlayerPage::aboutToHideEvent();
	temporaryPauseOnHide();
}

QString VideoPlayerPage::currentFileName(int index) const
{
	return file_list[index];
}

void VideoPlayerPage::temporaryPauseOnHide()
{
	if (!player->isPlaying())
		return;

	temporary_pause = true;
	pause();

	while (!player->isReallyPaused())
	{
		qDebug() << "Waiting for MPlayer to pause...";
		player->getPlayingInfo(500);
	}
	qDebug() << "MPlayer paused";
}

void VideoPlayerPage::resumePlayOnShow()
{
	if (temporary_pause && player->isPaused())
		MediaPlayerPage::resume();
	temporary_pause = false;
}

void VideoPlayerPage::previous()
{
	MediaPlayerPage::previous();

	if (player->isPaused())
	{
		player->requestInitialVideoInfo(currentFileName(current_file));
		title->setText(QFileInfo(file_list[current_file]).fileName());
	}
	else
		displayMedia(current_file);
}

void VideoPlayerPage::next()
{
	MediaPlayerPage::next();

	if (player->isPaused())
	{
		player->requestInitialVideoInfo(currentFileName(current_file));
		title->setText(QFileInfo(file_list[current_file]).fileName());
	}
	else
		displayMedia(current_file);
}

void VideoPlayerPage::resume()
{
	if (!isVisible() && !window->isVisible())
		return;

	MediaPlayerPage::resume();
}

void VideoPlayerPage::displayFullScreen()
{
	displayFullScreen(true);
}

void VideoPlayerPage::displayNoFullScreen()
{
	displayFullScreen(false);
}

void VideoPlayerPage::displayFullScreen(bool fs)
{
	player->quit();

	fullscreen = fs;
	if (fullscreen)
		window->showWindow();
	else
	{
		bt_global::page_stack.closeWindow(window);
		showPage();
	}

	QTimer::singleShot(0, this, SLOT(startMPlayer()));
}

QRect VideoPlayerPage::playbackGeometry()
{
	return QRect(video->mapToGlobal(video->rect().topLeft()), video->size());
}

void VideoPlayerPage::refreshPlayInfo(const QMap<QString,QString> &info)
{
	if (info.contains("current_time") && !info["current_time"].isEmpty())
	{
		QString timeS = info["current_time"];
		// remove after recompiling with the correct toolchain
		current_time = timeS.left(timeS.indexOf(QChar('.'))).toInt();
	}
}

void VideoPlayerPage::refreshPlayInfo()
{
	QMap<QString,QString> info = player->getVideoInfo();

	refreshPlayInfo(info);
}


// VideoPlayerWindow implementation
VideoPlayerWindow::VideoPlayerWindow(VideoPlayerPage *_page, MediaPlayer *player)
	: controls_timer(this)
{
	page = _page;

	controls = new QWidget;
	controls->hide();

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::VIDEO_WINDOW);

	ItemTuning *volume = 0;
	// if the touch is a sound diffusion source do not display the volume control
	if (!bt_global::audio_states->isSource())
		volume = new ItemTuning(QString(), bt_global::skin->getImage("volume"));

	QHBoxLayout *control_layout = new QHBoxLayout(controls);
	control_layout->setContentsMargins(0, 0, 0, 0);
	control_layout->addWidget(buttons);
	control_layout->addSpacing(50);
	if (volume)
		control_layout->addWidget(volume);

	QGridLayout *window_layout = new QGridLayout(this);
	window_layout->setContentsMargins(0, 0, 0, 0);
	window_layout->setColumnStretch(0, 1);
	window_layout->setColumnStretch(2, 1);
	window_layout->setRowStretch(0, 1);
	window_layout->addWidget(controls, 1, 1);

	// signals for navigation and to start/stop playback
	connect(buttons, SIGNAL(previous()), page, SLOT(previous()));
	connect(buttons, SIGNAL(next()), page, SLOT(next()));
	connect(buttons, SIGNAL(stop()), SIGNAL(Closed()));
	connect(buttons, SIGNAL(play()), page, SLOT(resume()));
	connect(buttons, SIGNAL(pause()), page, SLOT(pause()));
	connect(buttons, SIGNAL(seekForward()), page, SLOT(seekForward()));
	connect(buttons, SIGNAL(seekBack()), page, SLOT(seekBack()));

	// update the icon of the play button
	connect(player, SIGNAL(mplayerStarted()), buttons, SLOT(started()));
	connect(player, SIGNAL(mplayerDone()), buttons, SLOT(stopped()));
	connect(player, SIGNAL(mplayerStopped()), buttons, SLOT(stopped()));

	// reapint control buttons after MPlayer starts
	connect(player, SIGNAL(mplayerStarted()), SLOT(update()));
	connect(player, SIGNAL(mplayerResumed()), SLOT(update()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(update()));

	connect(buttons, SIGNAL(noFullScreen()), page, SLOT(displayNoFullScreen()));

	// timer to hide the buttons
	controls_timer.setSingleShot(true);
	connect(&controls_timer, SIGNAL(timeout()), controls, SLOT(hide()));

	// this shows the buttons when the user clicks the screen
	connect(this, SIGNAL(clicked()), SLOT(showButtons()));

	// these reset the buttons timer on each click
	connect(buttons, SIGNAL(previous()), SLOT(showButtons()));
	connect(buttons, SIGNAL(next()), SLOT(showButtons()));
	connect(buttons, SIGNAL(play()), SLOT(showButtons()));
	connect(buttons, SIGNAL(pause()), SLOT(showButtons()));

	if (volume)
	{
		connect(volume, SIGNAL(valueChanged(int)), SLOT(showButtons()));
		connect(volume, SIGNAL(valueChanged(int)), SLOT(setVolume(int)));
	}
}

void VideoPlayerWindow::mouseReleaseEvent(QMouseEvent *e)
{
	Window::mouseReleaseEvent(e);

	emit clicked();
}

void VideoPlayerWindow::showWindow()
{
	bt_global::page_stack.showUserWindow(this);
	Window::showWindow();
	page->resumePlayOnShow();
}

void VideoPlayerWindow::showButtons()
{
	controls_timer.stop();
	controls->show();
	controls_timer.start(BUTTONS_TIMEOUT);
}

void VideoPlayerWindow::setVolume(int value)
{
	bt_global::audio_states->setVolume(value);
}

void VideoPlayerWindow::aboutToHideEvent()
{
	Window::aboutToHideEvent();
	page->temporaryPauseOnHide();
}
