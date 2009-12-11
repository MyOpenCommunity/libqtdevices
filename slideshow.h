#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "page.h"

#include <QList>

class BtButton;
class QLabel;
class QTimer;


class SlideshowController : public QObject
{
Q_OBJECT
public:
	SlideshowController(QObject *parent);

	void initialize(int total, int current);
	bool slideshowActive();

public slots:
	void prevImageUser();
	void nextImageUser();
	void nextImageSlideshow();
	void startSlideshow();
	void stopSlideshow();

signals:
	void slideshowStarted();
	void slideshowStopped();
	void showImage(int);

private:
	int total_images, current_image;
	QTimer *timer;
};


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

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

private slots:
	void handleClose();
	void showImage(int image);

private:
	QLabel *title, *image;
	QList<QString> image_list;
	SlideshowController *controller;
};

#endif // SLIDESHOW_H
