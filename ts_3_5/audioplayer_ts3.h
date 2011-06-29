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

#ifndef AUDIOPLAYER_TS3_H
#define AUDIOPLAYER_TS3_H

#include "page.h"
#include "generic_functions.h" // EntryInfo

#include <QVector>
#include <QTime>


class StateButton;
class ScrollingLabel;
class MediaPlayer;
class ListManager;
class QTimer;


class AudioPlayerPage : public Page
{
Q_OBJECT
public:
	enum MediaType
	{
		IP_RADIO,
		UPNP_FILE,
		MAX_MEDIA_TYPE
	};

	static AudioPlayerPage *getAudioPlayerPage(MediaType type);
	static QVector<AudioPlayerPage *> audioPlayerPages();

	bool isPlayerInstanceRunning() const;
	bool isPlayerPaused() const;

signals:
	void loopDetected();
	void serverDown();

public slots:
	void playAudioFile(EntryInfo starting_file, int file_index, int num_files);

	void playAudioFiles(QList<QString> files, unsigned element);
	void playAudioFiles(EntryInfoList entries, unsigned element);
	void playAudioFilesBackground(QList<QString> files, unsigned element) {}

public slots:
	// standard player functionality
	//
	// note that previous()/next() wrap around at the start/end of the playlist
	//
	// as a convenience, stop() emits Closed()
	virtual void stop();
	virtual void resume();
	virtual void pause();
	virtual void previous();
	virtual void next();

	virtual void showPage();

private slots:
	void gotoSoundDiffusion();

	// update the play icon, and start/stop the timer to update the info
	void started();
	void stopped();

	void playToggled();

	void refreshPlayInfo(const QMap<QString, QString> &attrs);
	void refreshPlayInfo();

	void mplayerDone();
	void currentFileChanged();
	void resetLoopCheck();

	void handleServerDown();

private:
	static QVector<AudioPlayerPage *> audioplayer_pages;
	static const int MPLAYER_POLLING = 500;

	MediaPlayer *player;
	ListManager *list_manager;
	QTimer *refresh_data;
	StateButton *play_button;

	bool popup_mode; // true if the page must act as a popup using the page_stack
	int loop_starting_file; // the index of the song used to detect loop
	int loop_total_time; // the total time used to detect a loop
	QTime loop_time_counter; // used to count the time elapsed
	MediaType type;

	// Upnp info labels
	ScrollingLabel *track, *artist, *album, *length;
	// Radio ip labels
	ScrollingLabel *radio_url, *radio_title;
	bool radio_url_info, radio_title_info;

	AudioPlayerPage(MediaType type);
	void buildUi();
	void startPlayback();
	void startMPlayer(QString filename, int time);
	void clearLabels();
};

#endif // AUDIOPLAYER_TS3_H

