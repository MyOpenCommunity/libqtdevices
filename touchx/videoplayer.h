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


#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "mediaplayerpage.h"
#include "window.h"

#include <QTimer>

class QLabel;
class VideoPlayerWindow;


// page for video playback
class VideoPlayerPage : public MediaPlayerPage
{
Q_OBJECT
public:
	VideoPlayerPage();
	~VideoPlayerPage();

	virtual void showPage();

public slots:
	// displays the page and stores the video list for playback
	void displayVideos(QList<QString> videos, unsigned element);

signals:
	void started();
	void stopped();

protected:
	// kills mplayer
	void hideEvent(QHideEvent *event);

private:
	// the position/size where to play the video
	QRect playbackGeometry();
	void displayMedia(int index);
	void displayFullScreen(bool fullscreen);
	void startMPlayer(int index, int time);

private slots:
	void refreshPlayInfo();

	void displayFullScreen();
	void displayNoFullScreen();
	void playbackTerminated();
	void playbackStarted();
	void playbackStopped();

private:
	QLabel *title, *video;
	VideoPlayerWindow *window;
	int current_time;
	bool fullscreen;
};


// window for full screen video playback
class VideoPlayerWindow : public Window
{
Q_OBJECT
public:
	VideoPlayerWindow(VideoPlayerPage *videoplayer_page, MediaPlayer *mediaplayer);

signals:
	void clicked();

protected:
	void mouseReleaseEvent(QMouseEvent *e);

private slots:
	void showButtons();
	void setVolume(int value);

private:
	QWidget *controls;
	QTimer controls_timer;
};

#endif // VIDEOPLAYER_H
