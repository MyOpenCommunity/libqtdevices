#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "page.h"
#include "window.h"

#include <QTimer>

class QLabel;
class MediaPlayer;
class VideoPlayerWindow;
class MultimediaPlayerButtons;


// page for video playback
class VideoPlayerPage : public Page
{
Q_OBJECT
public:
	VideoPlayerPage();
	~VideoPlayerPage();

	virtual void showPage();

public slots:
	// displays the page and stores the video list for playback
	void displayVideos(QList<QString> videos, unsigned element);

signals:
	void started();
	void stopped();

protected:
	// kills mplayer
	void hideEvent(QHideEvent *event);

private:
	// the position/size where to play the video
	QRect playbackGeometry();
	void displayVideo(int index);
	void displayFullScreen(bool fullscreen);
	void startMPlayer(int index, int time);

private slots:
	void handleClose();
	void resume();
	void pause();
	void previous();
	void next();
	void skipForward();
	void skipBack();
	void refreshPlayInfo();
	void displayFullScreen();
	void displayNoFullScreen();
	void playbackStarted();
	void playbackStopped();
	void playbackTerminated();

private:
	QLabel *title, *video;
	int current_video, total_videos;
	QList<QString> video_list;
	MediaPlayer *player;
	VideoPlayerWindow *window;
	QTimer refresh_data;
	int current_time;
	bool fullscreen;
};


// window for full screen video playback
class VideoPlayerWindow : public Window
{
Q_OBJECT
public:
	VideoPlayerWindow(VideoPlayerPage *videoplayer_page, MediaPlayer *mediaplayer);

signals:
	void clicked();

protected:
	void mouseReleaseEvent(QMouseEvent *e);

private slots:
	void showButtons();
	void setVolume(int value);

private:
	QWidget *controls;
	QTimer controls_timer;
};

#endif // VIDEOPLAYER_H
