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


#ifndef MULTIMEDIA_BUTTONS_H
#define MULTIMEDIA_BUTTONS_H

#include <QWidget>

class BtButton;
class StateButton;


/*!
	\ingroup Multimedia
	\brief Common playback functionality (play/pause/stop/next/previous).
 */
class MultimediaPlayerButtons : public QWidget
{
Q_OBJECT
public:
	enum Type
	{
		IMAGE_PAGE = 1,
		IMAGE_WINDOW = 2,
		VIDEO_PAGE = 3,
		VIDEO_WINDOW = 4,
		AUDIO_PAGE = 5,
		IPRADIO_PAGE = 6,
	};

	MultimediaPlayerButtons(Type type);

	void showPrevButton(bool show);
	void showNextButton(bool show);

public slots:
	// update the icon of the play button
	void started();
	void stopped();

signals:
	void play();
	void pause();
	void stop();
	void previous();
	void next();
	void seekForward();
	void seekBack();
	void fullScreen();
	void noFullScreen();

private slots:
	void playToggled(bool playing);

private:
	BtButton *prev_button, *next_button, *stop_button, *rewind_button, *forward_button;
	StateButton *play_button;
	QString play_icon, pause_icon;

	BtButton *getButton(const QString &icon, const char *destination);
};

#endif // MULTIMEDIA_BUTTONS_H
