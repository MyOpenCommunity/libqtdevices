#include "videoplayer.h"
#include "slideshow.h" // TODO killme
#include "navigation_bar.h"
#include "mediaplayer.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFileInfo>


VideoPlayerPage::VideoPlayerPage()
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

	PlaybackButtons *buttons = new PlaybackButtons(PlaybackButtons::IN_PAGE);

	l->addWidget(title, 0, Qt::AlignHCenter);
	l->addLayout(v, 1);
	l->addWidget(buttons, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar(QString(), QString(), QString(), "back");
	buildPage(content, nav_bar);

	player = new MediaPlayer(this);

	// signals for navigation and to start/stop slideshow
	connect(buttons, SIGNAL(previous()), SLOT(previous()));
	connect(buttons, SIGNAL(next()), SLOT(next()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), SLOT(resume()));
	connect(buttons, SIGNAL(pause()), SLOT(pause()));
//	connect(buttons, SIGNAL(fullScreen()), SLOT(displayFullScreen()));

	// update the icon of the play button
	connect(this, SIGNAL(started()), buttons, SLOT(started()));
	connect(this, SIGNAL(stopped()), buttons, SLOT(stopped()));

	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));

	// close the page when the user clicks the stop button on the
	// full screen playback window
//	connect(window, SIGNAL(Closed()), SLOT(handleClose()));
}

void VideoPlayerPage::displayVideo(int index)
{
	title->setText(QFileInfo(video_list[index]).fileName());
	player->playVideo(video_list[index], playbackGeometry());
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

void VideoPlayerPage::hideEvent(QHideEvent *event)
{
	player->quit();
	emit stopped();
}

void VideoPlayerPage::handleClose()
{
	player->quit();
	emit stopped();
	emit Closed();
}

void VideoPlayerPage::resume()
{
	player->resume();\
	emit started();
}

void VideoPlayerPage::pause()
{
	player->pause();
	emit stopped();
}

void VideoPlayerPage::previous()
{
	player->quit();
	current_video -= 1;
	if (current_video < 0)
		current_video = total_videos - 1;

	displayVideo(current_video);
}

void VideoPlayerPage::next()
{
	player->quit();
	current_video += 1;
	if (current_video >= total_videos)
		current_video = 0;

	displayVideo(current_video);
}

QRect VideoPlayerPage::playbackGeometry()
{
	return QRect(video->mapToGlobal(video->rect().topLeft()), video->size());
}
