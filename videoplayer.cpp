#include "videoplayer.h"
#include "multimedia_buttons.h"
#include "navigation_bar.h"
#include "mediaplayer.h"
#include "displaycontrol.h" // forceOperativeMode
#include "hardware_functions.h" // setVolume
#include "bann2_buttons.h" // BannTuning
#include "skinmanager.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileInfo>
#include <QTime>
#include <QTimer>

#include <QtDebug>

#define BUTTONS_TIMEOUT 5000
#define MPLAYER_POLLING 500


// VideoPlayerPage implementation

VideoPlayerPage::VideoPlayerPage()
	: refresh_data(this)
{
	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);
	QHBoxLayout *v = new QHBoxLayout;

	// file name
	title = new QLabel("File name here");

	// only used for layout
	video = new QLabel;
	video->setFixedSize(320, 240);
	video->setStyleSheet("background: black");

	v->addStretch(1);
	v->addWidget(video);
	v->addStretch(1);

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::VIDEO_PAGE);

	l->addWidget(title, 0, Qt::AlignHCenter);
	l->addLayout(v, 1);
	l->addWidget(buttons, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar(QString(), QString(), QString(), "back");
	buildPage(content, nav_bar);

	player = new MediaPlayer(this);
	window = new VideoPlayerWindow(this, player);

	// signals for navigation and to start/stop playback
	connect(buttons, SIGNAL(previous()), SLOT(previous()));
	connect(buttons, SIGNAL(next()), SLOT(next()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), SLOT(resume()));
	connect(buttons, SIGNAL(pause()), SLOT(pause()));
	connect(buttons, SIGNAL(skipForward()), SLOT(skipForward()));
	connect(buttons, SIGNAL(skipBack()), SLOT(skipBack()));
	connect(buttons, SIGNAL(fullScreen()), SLOT(displayFullScreen()));

	// update the icon of the play button
	connect(this, SIGNAL(started()), buttons, SLOT(started()));
	connect(this, SIGNAL(stopped()), buttons, SLOT(stopped()));

	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));

	// close the page when the user clicks the stop button on the
	// full screen playback window
	connect(window, SIGNAL(Closed()), SLOT(handleClose()));

	// disable/reenable screen saver
	connect(this, SIGNAL(started()), SLOT(playbackStarted()));
	connect(this, SIGNAL(stopped()), SLOT(playbackStopped()));

	connect(&refresh_data, SIGNAL(timeout()), SLOT(refreshPlayInfo()));

	// handle mplayer termination
	connect(player, SIGNAL(mplayerDone()), SLOT(next()));
	connect(player, SIGNAL(mplayerAborted()), SLOT(playbackTerminated()));
	connect(player, SIGNAL(mplayerKilled()), SLOT(playbackTerminated()));

	// TODO for now use a fixed value
	setVolume(VOLUME_MMDIFFUSION, 3);
}

VideoPlayerPage::~VideoPlayerPage()
{
	player->quit();
	delete player;
}

void VideoPlayerPage::showPage()
{
	Page::showPage();
	fullscreen = false;
}

void VideoPlayerPage::startMPlayer(int index, int time)
{
	if (fullscreen)
		player->playVideoFullScreen(video_list[index], time);
	else
		player->playVideo(video_list[index], playbackGeometry(), time);
	refresh_data.start(MPLAYER_POLLING);
}

void VideoPlayerPage::displayVideo(int index)
{
	title->setText(QFileInfo(video_list[index]).fileName());
	startMPlayer(index, 0);
	emit started();
}

void VideoPlayerPage::displayVideos(QList<QString> videos, unsigned element)
{
	current_video = element;
	total_videos = videos.size();
	video_list = videos;
	showPage();

	displayVideo(current_video);
}

void VideoPlayerPage::playbackTerminated()
{
	emit stopped();
	refresh_data.stop();
	video->update();
	bt_global::display.forceOperativeMode(false);
}

void VideoPlayerPage::playbackStarted()
{
	bt_global::display.forceOperativeMode(true);
}

void VideoPlayerPage::playbackStopped()
{
	bt_global::display.forceOperativeMode(false);
}

void VideoPlayerPage::hideEvent(QHideEvent *event)
{
	if (fullscreen)
		return;

	player->pause();
	refresh_data.stop();
	emit stopped();
}

void VideoPlayerPage::handleClose()
{
	player->quit();
	refresh_data.stop();
	emit stopped();
	emit Closed();
}

void VideoPlayerPage::resume()
{
	if (player->isInstanceRunning())
	{
		player->resume();
		refresh_data.start(MPLAYER_POLLING);
	}
	else
		startMPlayer(current_video, 0);

	emit started();
}

void VideoPlayerPage::pause()
{
	player->pause();
	refresh_data.stop();
	emit stopped();
}

void VideoPlayerPage::previous()
{
	player->quitAndWait();
	current_video -= 1;
	if (current_video < 0)
		current_video = total_videos - 1;

	displayVideo(current_video);
}

void VideoPlayerPage::next()
{
	player->quitAndWait();
	current_video += 1;
	if (current_video >= total_videos)
		current_video = 0;

	displayVideo(current_video);
}

void VideoPlayerPage::skipForward()
{
	player->seek(10);
}

void VideoPlayerPage::skipBack()
{
	player->seek(-10);
}

void VideoPlayerPage::displayFullScreen()
{
	displayFullScreen(true);
}

void VideoPlayerPage::displayNoFullScreen()
{
	displayFullScreen(false);
}

void VideoPlayerPage::displayFullScreen(bool fs)
{
	player->quitAndWait();

	fullscreen = fs;
	if (fullscreen)
		window->showWindow();
	else
		showPage();

	startMPlayer(current_video, 0);
	emit started();
}

QRect VideoPlayerPage::playbackGeometry()
{
	return QRect(video->mapToGlobal(video->rect().topLeft()), video->size());
}

void VideoPlayerPage::refreshPlayInfo()
{
	QString timeS = player->getVideoInfo()["current_time"];
	if (timeS.isEmpty())
		return;

	// remove after recompiling with the correct toolchain
	current_time = timeS.left(timeS.indexOf(QChar('.'))).toInt();
}


// VideoPlayerWindow implementation

VideoPlayerWindow::VideoPlayerWindow(VideoPlayerPage *page, MediaPlayer *player)
	: controls_timer(this)
{
	controls = new QWidget;
	controls->hide();

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::VIDEO_WINDOW);
	BannTuning *volume = new BannTuning(QString(), bt_global::skin->getImage("volume"));

	QHBoxLayout *control_layout = new QHBoxLayout(controls);
	control_layout->setContentsMargins(0, 0, 0, 0);
	control_layout->addWidget(buttons);
	control_layout->addSpacing(50);
	control_layout->addWidget(volume);

	QGridLayout *window_layout = new QGridLayout(this);
	window_layout->setContentsMargins(0, 0, 0, 0);
	window_layout->setColumnStretch(0, 1);
	window_layout->setColumnStretch(2, 1);
	window_layout->setRowStretch(0, 1);
	window_layout->addWidget(controls, 1, 1);

	// signals for navigation and to start/stop playback
	connect(buttons, SIGNAL(previous()), page, SLOT(previous()));
	connect(buttons, SIGNAL(next()), page, SLOT(next()));
	connect(buttons, SIGNAL(stop()), SIGNAL(Closed()));
	connect(buttons, SIGNAL(play()), page, SLOT(resume()));
	connect(buttons, SIGNAL(pause()), page, SLOT(pause()));
	connect(buttons, SIGNAL(skipForward()), page, SLOT(skipForward()));
	connect(buttons, SIGNAL(skipBack()), page, SLOT(skipBack()));

	// update the icon of the play button
	connect(page, SIGNAL(started()), buttons, SLOT(started()));
	connect(page, SIGNAL(stopped()), buttons, SLOT(stopped()));

	// reapint control buttons after MPlayer starts
	connect(page, SIGNAL(started()), controls, SLOT(update()));
	connect(page, SIGNAL(stopped()), controls, SLOT(update()));

	connect(buttons, SIGNAL(noFullScreen()), page, SLOT(displayNoFullScreen()));

	// timer to hide the buttons
	controls_timer.setSingleShot(true);
	connect(&controls_timer, SIGNAL(timeout()), controls, SLOT(hide()));

	// this shows the buttons when the user clicks the screen
	connect(this, SIGNAL(clicked()), SLOT(showButtons()));

	// these reset the buttons timer on each click
	connect(buttons, SIGNAL(previous()), SLOT(showButtons()));
	connect(buttons, SIGNAL(next()), SLOT(showButtons()));
	connect(buttons, SIGNAL(play()), SLOT(showButtons()));
	connect(buttons, SIGNAL(pause()), SLOT(showButtons()));
	connect(volume, SIGNAL(valueChanged(int)), SLOT(showButtons()));

	// volume up/down
	connect(volume, SIGNAL(valueChanged(int)), SLOT(setVolume(int)));
}

void VideoPlayerWindow::mouseReleaseEvent(QMouseEvent *e)
{
	Window::mouseReleaseEvent(e);

	emit clicked();
}

void VideoPlayerWindow::showButtons()
{
	controls_timer.stop();
	controls->show();
	controls_timer.start(BUTTONS_TIMEOUT);
}

void VideoPlayerWindow::setVolume(int value)
{
	::setVolume(VOLUME_MMDIFFUSION, value);
}
