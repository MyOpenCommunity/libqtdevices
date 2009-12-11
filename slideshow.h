#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "page.h"

#include <QList>

class BtButton;
class QLabel;
class QTimer;


class PlaybackButtons : public QWidget
{
Q_OBJECT
public:
	enum Type
	{
		IN_PAGE = 1,
		IN_WINDOW = 2
	};

	PlaybackButtons(Type type);

public slots:
	void started();
	void stopped();

signals:
	void play();
	void pause();
	void stop();
	void previous();
	void next();
	void fullScreen();
	void noFullScreen();

private slots:
	void playToggled(bool playing);

private:
	BtButton *play_button;
	QString play_icon, pause_icon;
};


class SlideshowPage : public Page
{
Q_OBJECT
public:
	SlideshowPage();

public slots:
	void displayImages(QList<QString> images, unsigned element);

signals:
	void slideshowStarted();
	void slideshowStopped();

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

private:
	void showCurrentImage();
	bool slideshowActive();

private slots:
	void prevImageUser();
	void nextImageUser();
	void nextImageSlideshow();
	void handleClose();
	void startSlideshow();
	void stopSlideshow();

private:
	QLabel *title, *image;
	QList<QString> image_list;
	int current_image;
	QTimer *timer;
};

#endif // SLIDESHOW_H
