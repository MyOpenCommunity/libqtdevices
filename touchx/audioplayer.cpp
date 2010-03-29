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

#include <QGridLayout>
#include <QLabel>
#include <QVariant> // for setProperty


AudioPlayerPage::AudioPlayerPage(MediaType t)
{
	type = t;

	QWidget *content = new QWidget;
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);

	buildPage(content, nav_bar, QString(), TINY_TITLE_HEIGHT);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	QLabel *bg = new QLabel;
	QGridLayout *l_bg = new QGridLayout(bg);

	bg->setPixmap(bt_global::skin->getImage("audioplayer_background"));

	description_top = new QLabel;
	description_bottom = new QLabel;
	description_top->setFont(bt_global::font->get(FontManager::PLAYER_TITLE));
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
	BtButton *goto_sounddiff = new BtButton(bt_global::skin->getImage("goto_sounddiffusion"));
	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(type == IP_RADIO ? MultimediaPlayerButtons::IPRADIO_PAGE : MultimediaPlayerButtons::AUDIO_PAGE);

	connectMultimediaButtons(buttons);

	l_btn->addWidget(buttons);
	l_btn->addStretch(0);
	l_btn->addWidget(goto_sounddiff);

	ItemTuning *volume = new ItemTuning(tr("Volume"), bt_global::skin->getImage("volume"));
	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));

	QVBoxLayout *l = new QVBoxLayout(content);
	l->addWidget(bg, 1, Qt::AlignCenter);
	l->addLayout(l_btn, 1);
	l->addWidget(volume, 1, Qt::AlignCenter);

	connect(&refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));
}

void AudioPlayerPage::startMPlayer(int index, int time)
{
	if (type == IP_RADIO)
	{
		description_top->setText(tr("Loading..."));
		description_bottom->setText(tr("Loading..."));
	}
	else
	{
		description_top->setText(tr(" "));
		description_bottom->setText(tr(" "));
	}

	player->play(file_list[index], true);
	refresh_data.start(MPLAYER_POLLING);
}

void AudioPlayerPage::displayMedia(int index)
{
	track->setText(tr("Track: %1 / %2").arg(index + 1).arg(total_files));
	startMPlayer(index, 0);
	emit started();
}

void AudioPlayerPage::playAudioFiles(QList<QString> files, unsigned element)
{
	current_file = element;
	total_files = files.size();
	file_list = files;
	showPage();

	displayMedia(current_file);
}

// strips the decimal dot from the time returned by mplayer; if match_length is passed,
// the result is left-padded with "00:" to match match_length length
static QString formatTime(const QString &mp_time, const QString &match_length = QString())
{
	QString res = mp_time;
	int dot = mp_time.indexOf('.');

	// strip decimal point
	if (dot > 0)
		res = mp_time.left(dot);

	// left-pad with "00:"
	while (res.length() < match_length.length())
		res = "00:" + res;

	return res;
}

void AudioPlayerPage::refreshPlayInfo()
{
	QMap<QString, QString> attrs = player->getPlayingInfo();

	if (type == LOCAL_FILE)
	{
		if (attrs.contains("meta_title"))
			description_top->setText(attrs["meta_title"]);
		else if (attrs.contains("file_name"))
			description_top->setText(attrs["file_name"]);

		if (attrs.contains("meta_artist"))
			description_bottom->setText(attrs["meta_artist"]);
		else if (attrs.contains("meta_album"))
			description_bottom->setText(attrs["meta_album"]);
	}
	else if (type == IP_RADIO)
	{
		 if (attrs.contains("stream_url"))
			 description_top->setText(attrs["stream_url"]);

		 if (attrs.contains("stream_title"))
			 description_bottom->setText(attrs["stream_title"]);
	}

	if (type == LOCAL_FILE && attrs.contains("total_time") && attrs.contains("current_time"))
	{
		QString total = formatTime(attrs["total_time"]);
		QString current = formatTime(attrs["current_time"], total);

		elapsed->setText(current + " / " + total);
	}
}

void AudioPlayerPage::changeVolume(int volume)
{
	setVolume(VOLUME_MMDIFFUSION, volume);
}
