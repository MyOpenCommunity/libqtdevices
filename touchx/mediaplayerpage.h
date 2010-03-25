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


#ifndef MEDIAPLAYERPAGE_H
#define MEDIAPLAYERPAGE_H

#include "page.h"

#include <QTimer>

class MediaPlayer;
class MultimediaPlayerButtons;


class MediaPlayerPage : public Page
{
Q_OBJECT
public:
	static const int MPLAYER_POLLING = 500;

	MediaPlayerPage();

signals:
	void started();
	void stopped();

protected:
	void connectMultimediaButtons(MultimediaPlayerButtons *buttons);

	virtual void displayMedia(int index) = 0;

	virtual void playbackTerminated();

protected slots:
	virtual void stop();
	virtual void resume();
	virtual void pause();
	virtual void previous();
	virtual void next();
	virtual void seekForward();
	virtual void seekBack();

protected:
	int current_file, total_files;
	QList<QString> file_list;
	MediaPlayer *player;
	QTimer refresh_data;
};

#endif // MEDIAPLAYERPAGE_H
