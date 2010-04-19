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

class QLabel;
class AudioPlayerPage;


/*
 * Tray icon displayed in the header when either an audio file or a web radio
 * is playing.  Clicking the icon jumps to the player page.
 */
class AudioPlayerTray : public BtButton
{
Q_OBJECT
public:
	AudioPlayerTray(const QString &icon);

public slots:
	void stopped();
	void started();

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
	};

	AudioPlayerPage(MediaType type);

public slots:
	void playAudioFiles(QList<QString> images, unsigned element);

private:
	void startMPlayer(int index, int time);
	void displayMedia(int index);

private slots:
	void refreshPlayInfo();
	void changeVolume(int volume);

private:
	MediaType type;
	QLabel *description_top, *description_bottom, *track, *elapsed;
	// icon in tray bar
	static AudioPlayerTray *tray_icon;
};

#endif
