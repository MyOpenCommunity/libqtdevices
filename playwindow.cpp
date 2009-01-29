/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** playwindow.cpp
 **
 ** finestra di dati sulla sorgente MultimediaSource
 **
 ****************************************************************/


#include "playwindow.h"
#include "titlelabel.h"
#include "mediaplayer.h"
#include "fontmanager.h" // bt_global::font
#include "buttons_bar.h"
#include "main.h"

#include <QLayout>

#include <unistd.h>

/*
 * Interface icon paths.
 */
static const char *IMG_PLAY = IMG_PATH "btnplay.png"; 
static const char *IMG_STOP = IMG_PATH "btnsdstop.png";
static const char *IMG_PAUSE = IMG_PATH "btnpause.png";
static const char *IMG_NEXT = IMG_PATH "btnforward.png";
static const char *IMG_PREV = IMG_PATH "btnbackward.png";
static const char *IMG_BACK = IMG_PATH "arrlf.png";
static const char *IMG_SETTINGS = IMG_PATH "appdiffsmall.png";

/*
 * Path in conf.xml where the configurable label texts are found.
 */
#define CFG_LABELS_MEDIAPLAYER "configuratore/setup/labels/mediaplayer/"


/// ***********************************************************************************************************************
/// Methods for PlayWindow
/// ***********************************************************************************************************************

PlayWindow::PlayWindow(MediaPlayer *player, QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::Window)
{
	current_track = CURRENT_TRACK_NONE;
	read_player_output = true;

	/// set self Geometry
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);

	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 30, 0, 10);
	media_player = player;

	connect(media_player, SIGNAL(mplayerDone()), SLOT(handlePlayingDone()));
	connect(media_player, SIGNAL(mplayerKilled()), SLOT(handlePlayingKilled()));
	connect(media_player, SIGNAL(mplayerAborted()), SLOT(handlePlayingAborted()));

	addMainControls();
}

void PlayWindow::addMainControls()
{
	QHBoxLayout *layout = new QHBoxLayout();
	back_btn = new BtButton();
	settings_btn = new BtButton();
	layout->addWidget(back_btn, 0, Qt::AlignLeft);
	layout->addWidget(settings_btn, 0, Qt::AlignRight);

	back_btn->setImage(IMG_BACK);
	settings_btn->setImage(IMG_SETTINGS);

	main_layout->addStretch();
	main_layout->addLayout(layout);

	connect(back_btn, SIGNAL(released()), SIGNAL(backBtn()));
	connect(settings_btn, SIGNAL(released()), SIGNAL(settingsBtn()));
}

void PlayWindow::prevTrack()
{
	if (media_player->isInstanceRunning() && current_track != 0)
	{
		stopPlayer();
		startPlayer(current_track - 1);

		qDebug("[AUDIO] PlayWindow::prevTrack() now playing: %u/%u", current_track, play_list.count() - 1);
	}
}

void PlayWindow::nextTrack()
{
	if (media_player->isInstanceRunning() && static_cast<int>(current_track) < (play_list.size() - 1))
	{
		playNextTrack();
		qDebug("[AUDIO] PlayWindow::nextTrack() now playing: %u/%u", current_track, play_list.count() - 1);
	}
}

void PlayWindow::playNextTrack()
{
	stopPlayer();
	startPlayer(current_track + 1);
}

void PlayWindow::pause()
{
	qDebug("[AUDIO] media_player: pause play");
	if (media_player->isInstanceRunning())
		media_player->pause();
}

void PlayWindow::stop()
{
	qDebug("[AUDIO] PlayWindow::stop()");
	stopPlayer();
	current_track = CURRENT_TRACK_NONE;
}

void PlayWindow::resume()
{
	qDebug("[AUDIO] media_player: resume play");
	if (media_player->isInstanceRunning())
		media_player->resume();
}

bool PlayWindow::isPlaying()
{
	return media_player->isInstanceRunning();
}

void PlayWindow::handlePlayingDone()
{
	/*
	 * mplayer has terminated because the track is finished
	 * so we go to the next track if exists.
	 */
	if (static_cast<int>(current_track) < (play_list.size() - 1))
	{
		playNextTrack();
	}
	else
		startPlayer(0);
}

void PlayWindow::handlePlayingAborted()
{
	emit notifyStopPlay();

	// FIXME display error?
	qDebug("[AUDIO] Error in mplayer, stopping playlist");
}

void PlayWindow::startPlayer(QVector<AudioData> _play_list, unsigned element)
{
	qDebug("[AUDIO] startPlayer()");
	stop();
	play_list = _play_list;
	startPlayer(element);
}

void PlayWindow::startPlayer(unsigned int track)
{
	// Start playing and point next Track
	current_track = track;

	qDebug("[AUDIO] start new mplayer instance with current_track=%u", current_track);
	media_player->play(play_list[current_track].path, read_player_output);
}

void PlayWindow::stopPlayer()
{
	// quit mplayer if it is already playing
	if (media_player->isInstanceRunning())
	{
		/*
		 * After stop() and before starting a new istance,  we should wait
		 * for the SIGCHLD signal emitted after quits.
		 * usleep() exits immediately with EINTR error in case of signal.
		 */
		media_player->quit();
		qDebug("[AUDIO] PlayWindow: waiting for mplayer to exit...");
		usleep(1000000);
		qDebug("[AUDIO] Ok");
	}
}

QString PlayWindow::getCurrentDescription()
{
	return play_list[current_track].desc;
}


/// ***********************************************************************************************************************
/// Methods for MediaPlayWindow
/// ***********************************************************************************************************************

MediaPlayWindow::MediaPlayWindow(MediaPlayer *player, QWidget *parent) : PlayWindow(player, parent)
{
	qDebug("[AUDIO] MediaPlayWindow costructor");

	/// Create Labels (that contain tags)
	QFont aFont = bt_global::font.get(FontManager::TEXT);

	// layouts for media
	QHBoxLayout *tags_layout = new QHBoxLayout();
	main_layout->insertLayout(0, tags_layout);
	main_layout->insertStretch(1);

	QVBoxLayout *tags_name_layout = new QVBoxLayout();
	tags_name_layout->setContentsMargins(10, 0, 0, 0);
	QVBoxLayout *tags_text_layout = new QVBoxLayout();

	addNameLabels(tags_name_layout, aFont);
	addTextLabels(tags_text_layout, aFont);

	tags_layout->addLayout(tags_name_layout);
	tags_layout->addLayout(tags_text_layout);

	play_controls = new ButtonsBar(this, 4, Qt::Horizontal);
	play_controls->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/(NUM_RIGHE+1), MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	play_controls->setButtonIcon(1, IMG_STOP);
	play_controls->setButtonIcon(2, IMG_PREV);
	play_controls->setButtonIcon(3, IMG_NEXT);

	main_layout->insertWidget(2, play_controls);
	main_layout->insertStretch(3);

	connect(play_controls, SIGNAL(clicked(int)), SLOT(handleButtons(int)));

	data_refresh_timer = new QTimer(this);

	// Connect the timer to the handler, the timer get and displays data from mplayer
	connect(data_refresh_timer, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

	// Set Timer
	refresh_time = 500;
}

void MediaPlayWindow::showPlayBtn()
{
	play_controls->setButtonIcon(0, IMG_PLAY);
}

void MediaPlayWindow::showPauseBtn()
{
	play_controls->setButtonIcon(0, IMG_PAUSE);
}

void MediaPlayWindow::addNameLabels(QBoxLayout *layout, QFont& aFont)
{
	/// Create Labels (that contain tag names)
	QString label_a = tr("Track:");
	QString label_b = tr("Artist:");
	QString label_c = tr("Album:");
	QString label_d = tr("Length:");

	// Set label names
	TitleLabel *name_label   = new TitleLabel(this, MAX_WIDTH/3, 30, 9, 0);
	TitleLabel *artist_label = new TitleLabel(this, MAX_WIDTH/3, 30, 9, 0);
	TitleLabel *album_label  = new TitleLabel(this, MAX_WIDTH/3, 30, 9, 0);
	TitleLabel *time_label   = new TitleLabel(this, MAX_WIDTH/3, 30, 9, 0);

	// set font
	name_label->setFont(aFont);
	artist_label->setFont(aFont);
	album_label->setFont(aFont);
	time_label->setFont(aFont);

	// set text
	name_label->setText(label_a);
	artist_label->setText(label_b);
	album_label->setText(label_c);
	time_label->setText(label_d);

	// add all labels to layouts
	layout->addWidget(name_label);
	layout->addWidget(artist_label);
	layout->addWidget(album_label);
	layout->addWidget(time_label);
}

void MediaPlayWindow::addTextLabels(QBoxLayout *layout, QFont& aFont)
{
	// create Labels containing INFO
	meta_title_label  = new TitleLabel(this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE);
	meta_artist_label = new TitleLabel(this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE);
	meta_album_label  = new TitleLabel(this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE);
	time_pos_label    = new TitleLabel(this, MAX_WIDTH - MAX_WIDTH/3, 30, 9, 0, FALSE);

	// Set Font
	meta_title_label->setFont(aFont);
	meta_artist_label->setFont(aFont);
	meta_album_label->setFont(aFont);
	time_pos_label->setFont(aFont);

	layout->addWidget(meta_title_label);
	layout->addWidget(meta_artist_label);
	layout->addWidget(meta_album_label);
	layout->addWidget(time_pos_label);
}

void MediaPlayWindow::startPlayer(unsigned int track)
{
	PlayWindow::startPlayer(track);
	cleanPlayInfo();
	showPauseBtn();
	data_refresh_timer->start(refresh_time);
}

void MediaPlayWindow::stopPlayer()
{
	PlayWindow::stopPlayer();
	showPlayBtn();
	data_refresh_timer->stop();
}

void MediaPlayWindow::refreshPlayInfo()
{
	QMap<QString, QString> updated_playing_info;
	updated_playing_info = media_player->getPlayingInfo();

	// Now we iterate on updated_playing_info and import new entries in playing_info
	QMap<QString, QString>::Iterator it;
	for (it = updated_playing_info.begin(); it != updated_playing_info.end(); ++it)
		playing_info[it.key()] = it.value();

	// Extract Time Data
	QStringList total   = playing_info["total_time"].split(".");
	QStringList current = playing_info["current_time"].split(".");

	// Set INFO in Labels
	if (playing_info["meta_title"].isNull())
		playing_info["meta_title"] = getCurrentDescription();

	meta_title_label->setText(playing_info["meta_title"]);
	meta_artist_label->setText(playing_info["meta_artist"]);
	meta_album_label->setText(playing_info["meta_album"]);
	if (playing_info["total_time"] == "" || playing_info["current_time"] == "")
		time_pos_label->setText(QString("- ") + tr("of") + QString(" -"));
	else
		time_pos_label->setText(QString("%1 of %2").arg(current[0]).arg(total[0]));
}

void MediaPlayWindow::cleanPlayInfo()
{
	meta_title_label->setText(QString("-"));
	meta_artist_label->setText(QString("-"));
	meta_album_label->setText(QString("-"));
	time_pos_label->setText(QString("- of -"));
	playing_info.clear();
}

void MediaPlayWindow::pause()
{
	if (isPlaying())
	{
		PlayWindow::pause();
		data_refresh_timer->stop();
		showPlayBtn();
	}
}

void MediaPlayWindow::resume()
{
	if (isPlaying())
	{
		PlayWindow::resume();
		data_refresh_timer->start(refresh_time);
		showPauseBtn();
	}
}

void MediaPlayWindow::handlePlayingDone()
{
	cleanPlayInfo();
	data_refresh_timer->stop();
	PlayWindow::handlePlayingDone();
}

void MediaPlayWindow::handlePlayingKilled()
{
	cleanPlayInfo();
	data_refresh_timer->stop();
	PlayWindow::handlePlayingKilled();
}

void MediaPlayWindow::handleButtons(int button_number)
{
	switch (button_number)
	{
	case 0:
		if (!isPlaying())
		{
			qDebug("[AUDIO] media_player: start from track 0");
			startPlayer(0);
		}
		else if (media_player->isPaused())
			resume();
		else
			pause();
		break;
	case 1:
		stop();
		break;
	case 2:
		prevTrack();
		break;
	case 3:
		nextTrack();
		break;
	}
}

/// ***********************************************************************************************************************
/// Methods for RadioPlayWindow
/// ***********************************************************************************************************************

RadioPlayWindow::RadioPlayWindow(MediaPlayer *player, QWidget *parent) : PlayWindow(player, parent)
{
	qDebug("[AUDIO] RadioPlayWindow costructor");
	read_player_output = false;
	//main_layout->insertSpacing(0, 20);

	/// Create Labels (that contain tags)

	meta_title_label = new TitleLabel(this, MAX_WIDTH, 30, 0, 0, TRUE);
	meta_title_label->setFont(bt_global::font.get(FontManager::TEXT));
	meta_title_label->setAlignment(Qt::AlignHCenter);
	main_layout->insertWidget(0, meta_title_label);
	main_layout->insertStretch(1);

	play_controls = new ButtonsBar(this, 4, Qt::Horizontal);
	play_controls->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/(NUM_RIGHE+1), MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	play_controls->setButtonIcon(0, IMG_PLAY);
	play_controls->setButtonIcon(1, IMG_STOP);
	play_controls->setButtonIcon(2, IMG_PREV);
	play_controls->setButtonIcon(3, IMG_NEXT);

	main_layout->insertWidget(2, play_controls);
	main_layout->insertStretch(3);
	connect(play_controls, SIGNAL(clicked(int)), SLOT(handleButtons(int)));
}

void RadioPlayWindow::startPlayer(unsigned int track)
{
	PlayWindow::startPlayer(track);
	meta_title_label->setText(getCurrentDescription());
}

void RadioPlayWindow::handleButtons(int button_number)
{
	switch (button_number)
	{
	case 0:
		if (!isPlaying())
		{
			qDebug("[AUDIO] radio_player: start from track 0");
			startPlayer(0);
		}
		break;
	case 1:
		stop();
		break;
	case 2:
		prevTrack();
		break;
	case 3:
		nextTrack();
		break;
	}
}
