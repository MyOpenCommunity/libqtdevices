#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "page.h"

class QLabel;
class MediaPlayer;


// page for video playback
class VideoPlayerPage : public Page
{
Q_OBJECT
public:
	VideoPlayerPage();

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

private slots:
	void handleClose();
	void resume();
	void pause();
	void previous();
	void next();

private:
	QLabel *title, *video;
	int current_video, total_videos;
	QList<QString> video_list;
	MediaPlayer *player;
};

#endif // VIDEOPLAYER_H
