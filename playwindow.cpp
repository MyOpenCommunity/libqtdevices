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
#include "bannondx.h"
#include "fontmanager.h"
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

static const char *IMG_PLAY_P = IMG_PATH "btnplayp.png"; 
static const char *IMG_STOP_P = IMG_PATH "btnsdstopp.png";
static const char *IMG_PAUSE_P = IMG_PATH "btnpausep.png";
static const char *IMG_NEXT_P = IMG_PATH "btnforwardp.png";
static const char *IMG_PREV_P = IMG_PATH "btnbackwardp.png";
static const char *IMG_BACK_P = IMG_PATH "arrlfp.png";
static const char *IMG_SETTINGS_P = IMG_PATH "appdiffsmallp.png";

/*
 * Path in conf.xml where the configurable label texts are found.
 */
#define CFG_LABELS_MEDIAPLAYER "configuratore/setup/labels/mediaplayer/"


/// ***********************************************************************************************************************
/// Methods for PlayWindow
/// ***********************************************************************************************************************

PlayWindow::PlayWindow(MediaPlayer *player, QWidget *parent, const char * name) :
	QWidget(parent, Qt::FramelessWindowHint | Qt::Window)
{
	current_track = CURRENT_TRACK_NONE;
	read_player_output = true;

	/// set self Geometry
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);

	/// Create Main Layout
	// all others layout must have this as parent, this is not more needed in Qt4
	// where we can use setMainLayout
	main_layout = new QVBoxLayout(this);
	media_player = player;

	connect(media_player, SIGNAL(mplayerDone()), SLOT(handlePlayingDone()));
	connect(media_player, SIGNAL(mplayerKilled()), SLOT(handlePlayingKilled()));
	connect(media_player, SIGNAL(mplayerAborted()), SLOT(handlePlayingAborted()));

	QHBoxLayout *main_controls_layout = new QHBoxLayout();
	addMainControls(main_controls_layout);
	main_layout->addLayout(main_controls_layout);
}

void PlayWindow::addMainControls(QBoxLayout* layout)
{
	back_btn = new BtButton(this);
	settings_btn = new BtButton(this);
	layout->addWidget(back_btn);
	layout->addStretch();
	layout->addWidget(settings_btn);

	back_btn->setImage(IMG_BACK);
	settings_btn->setImage(IMG_SETTINGS);

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

MediaPlayWindow::MediaPlayWindow(MediaPlayer *player, QWidget *parent, const char * name) :
	PlayWindow(player, parent, name)
{
	qDebug("[AUDIO] MediaPlayWindow costructor");
	main_layout->insertSpacing(0, 20);

	/// Create Labels (that contain tags)
	QFont aFont;
	FontManager::instance()->getFont(font_playwindow, aFont);

	// layouts for media
	QHBoxLayout *tags_layout = new QHBoxLayout();
	main_layout->insertLayout(1, tags_layout);

	QVBoxLayout *tags_name_layout = new QVBoxLayout();
	QVBoxLayout *tags_text_layout = new QVBoxLayout();

	addNameLabels(tags_name_layout, aFont);
	addTextLabels(tags_text_layout, aFont);

	tags_layout->addLayout(tags_name_layout);
	tags_layout->addLayout(tags_text_layout);

	play_controls = new ButtonsBar(this, 4, Qt::Horizontal);
	play_controls->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/(NUM_RIGHE+1), MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	/*
	 * Create Buttons and set their geometry
	 */
	QPixmap *icon;
	QPixmap *pressed_icon;

	icon         = icons_library.getIcon(IMG_STOP);
	pressed_icon = icons_library.getIcon(IMG_STOP_P);
	play_controls->setButtonIcons(1, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_PREV);
	pressed_icon = icons_library.getIcon(IMG_PREV_P);
	play_controls->setButtonIcons(2, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_NEXT);
	pressed_icon = icons_library.getIcon(IMG_NEXT_P);
	play_controls->setButtonIcons(3, *icon, *pressed_icon);

	main_layout->insertWidget(2, play_controls);
	// Add space to the end of layout to align buttons with previus page
	main_layout->addSpacing(10);
	connect(play_controls, SIGNAL(clicked(int)), SLOT(handleButtons(int)));

	data_refresh_timer = new QTimer(this);

	// Connect the timer to the handler, the timer get and displays data from mplayer
	connect(data_refresh_timer, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

	// Set Timer
	refresh_time = 500;
}

void MediaPlayWindow::showPlayBtn()
{
	QPixmap *icon         = icons_library.getIcon(IMG_PLAY);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_PLAY_P);
	play_controls->setButtonIcons(0, *icon, *pressed_icon);
}

void MediaPlayWindow::showPauseBtn()
{
	QPixmap *icon         = icons_library.getIcon(IMG_PAUSE);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_PAUSE_P);
	play_controls->setButtonIcons(0, *icon, *pressed_icon);
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

RadioPlayWindow::RadioPlayWindow(MediaPlayer *player, QWidget *parent, const char * name) :
	PlayWindow(player, parent, name)
{
	qDebug("[AUDIO] RadioPlayWindow costructor");
	read_player_output = false;
	main_layout->insertSpacing(0, 20);

	/// Create Labels (that contain tags)
	QFont aFont;
	FontManager::instance()->getFont(font_playwindow, aFont);

	// layouts for media
	QVBoxLayout *tags_layout = new QVBoxLayout();
	main_layout->insertLayout(1, tags_layout);
	meta_title_label = new TitleLabel(this, MAX_WIDTH, 30, 0, 0, TRUE);
	meta_title_label->setFont(aFont);
	meta_title_label->setAlignment(Qt::AlignHCenter);
	tags_layout->addWidget(meta_title_label);

	play_controls = new ButtonsBar(this, 4, Qt::Horizontal);
	play_controls->setGeometry(0, MAX_HEIGHT - MAX_HEIGHT/(NUM_RIGHE+1), MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE);

	 // Create Buttons and set their geometry
	QPixmap *icon;
	QPixmap *pressed_icon;

	icon         = icons_library.getIcon(IMG_PLAY);
	pressed_icon = icons_library.getIcon(IMG_PLAY_P);
	play_controls->setButtonIcons(0, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_STOP);
	pressed_icon = icons_library.getIcon(IMG_STOP_P);
	play_controls->setButtonIcons(1, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_PREV);
	pressed_icon = icons_library.getIcon(IMG_PREV_P);
	play_controls->setButtonIcons(2, *icon, *pressed_icon);

	icon         = icons_library.getIcon(IMG_NEXT);
	pressed_icon = icons_library.getIcon(IMG_NEXT_P);
	play_controls->setButtonIcons(3, *icon, *pressed_icon);

	main_layout->insertWidget(2, play_controls);
	// Add space to the end of layout to align buttons with previus page
	main_layout->addSpacing(10);
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
