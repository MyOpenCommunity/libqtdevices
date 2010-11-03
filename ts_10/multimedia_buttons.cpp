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


#include "multimedia_buttons.h"
#include "state_button.h"
#include "skinmanager.h"

#include <QHBoxLayout>


MultimediaPlayerButtons::MultimediaPlayerButtons(Type type)
{
	bool is_window = type == IMAGE_WINDOW || type == VIDEO_WINDOW;

	QHBoxLayout *buttons_layout = new QHBoxLayout(this);
	buttons_layout->setContentsMargins(0, 0, 0, 0);
	buttons_layout->setSpacing(is_window ? 0 : 5);

	play_icon = bt_global::skin->getImage("start");
	pause_icon = bt_global::skin->getImage("pause");

	prev_button = getButton("previous", SIGNAL(previous()));
	next_button = getButton("next", SIGNAL(next()));
	stop_button = getButton("stop", SIGNAL(stop()));

	forward_button = 0;
	rewind_button = 0;
	BtButton *screen = NULL;

	if (type != AUDIO_PAGE && type != IPRADIO_PAGE)
		screen = getButton(is_window ? "nofullscreen" : "fullscreen",
				   is_window ? SIGNAL(noFullScreen()) : SIGNAL(fullScreen()));

	if (type == VIDEO_PAGE || type == VIDEO_WINDOW || type == AUDIO_PAGE)
	{
		forward_button = getButton("skip_forward", SIGNAL(seekForward()));
		rewind_button = getButton("skip_back", SIGNAL(seekBack()));
	}

	play_button = new StateButton;
	play_button->setOffImage(play_icon);
	play_button->setOnImage(pause_icon);
	play_button->setCheckable(true);
	play_button->setStatus(false);
	connect(play_button, SIGNAL(clicked(bool)), SLOT(playToggled(bool)));

	buttons_layout->addWidget(prev_button);
	if (rewind_button)
		buttons_layout->addWidget(rewind_button);
	buttons_layout->addWidget(play_button);
	buttons_layout->addWidget(stop_button);
	if (forward_button)
		buttons_layout->addWidget(forward_button);
	buttons_layout->addWidget(next_button);
	if (screen)
		buttons_layout->addWidget(screen);
}

void MultimediaPlayerButtons::showPrevButton(bool show)
{
	prev_button->setVisible(show);
}

void MultimediaPlayerButtons::showNextButton(bool show)
{
	next_button->setVisible(show);
}

BtButton *MultimediaPlayerButtons::getButton(const QString &icon, const char *destination)
{
	BtButton *button = new BtButton;
	button->setImage(bt_global::skin->getImage(icon));
	QObject::connect(button, SIGNAL(clicked()), destination);

	return button;
}

void MultimediaPlayerButtons::playToggled(bool playing)
{
	if (playing)
		emit play();
	else
		emit pause();
}

void MultimediaPlayerButtons::started()
{
	play_button->setStatus(true);
	play_button->setChecked(true);
}

void MultimediaPlayerButtons::stopped()
{
	play_button->setStatus(false);
	play_button->setChecked(false);
}

