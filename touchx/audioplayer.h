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


#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "mediaplayerpage.h"
#include "btbutton.h"

#include <QTime>

class QLabel;
class AudioPlayerPage;
class VirtualSourceDevice;


/*
 * Tray icon displayed in the header when either an audio file or a web radio
 * is playing.  Clicking the icon jumps to the player page.
 */
class AudioPlayerTray : public BtButton
{
Q_OBJECT
public:
	AudioPlayerTray(const QString &icon);

	void setCurrentPlayer(AudioPlayerPage *player);
	AudioPlayerPage *currentPlayer() const;

private slots:
	void gotoPlayer();

private:
	AudioPlayerPage *current_player;
};


/*
 * Page that can be used to play both audio files (es. MP3) and ShoutCast
 * streams (for IP radio).
 */
class AudioPlayerPage : public MediaPlayerPage
{
Q_OBJECT
public:
	enum MediaType
	{
		LOCAL_FILE,
		IP_RADIO,
		MAX_MEDIA_TYPE
	};

	static AudioPlayerPage *getAudioPlayerPage(MediaType type);
	static QVector<AudioPlayerPage *> audioPlayerPages();

	virtual int sectionId() const;

protected:
	QString currentFileName(int index) const;

public slots:
	void playAudioFiles(QList<QString> files, unsigned element);
	void playAudioFilesBackground(QList<QString> files, unsigned element);
	virtual void previous();
	virtual void next();

private:
	void startMPlayer(int index, int time);
	void displayMedia(int index);
	void clearLabels();

private slots:
	void refreshPlayInfo(const QMap<QString, QString> &attrs);
	void refreshPlayInfo();
	void changeVolume(int volume);
	void gotoSoundDiffusion();
	void showTrayIcon();
	void hideTrayIcon();

private:
	AudioPlayerPage(MediaType type);

	MediaType type;
	QLabel *description_top, *description_bottom, *track, *elapsed;
	// icon in tray bar
	static AudioPlayerTray *tray_icon;
	static QVector<AudioPlayerPage *> audioplayer_pages;

	int loop_starting_file, loop_total_time;
	QTime loop_time_counter;
};

#endif
