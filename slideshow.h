#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "page.h"
#include "window.h"

#include <QList>
#include <QLabel>

class BtButton;
class QLabel;
class QTimer;
class SlideshowWindow;


class SlideshowController : public QObject
{
Q_OBJECT
public:
	SlideshowController(QObject *parent);

	void initialize(int total, int current);
	bool slideshowActive();
	int currentImage();

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


// common code to display an image
class SlideshowImage : public QLabel
{
Q_OBJECT
public:
	SlideshowImage();

	// if the pixmap is too big for the label, it is scaled down, respecting
	// proportions, otherwise it is displayed as it is
	void setPixmap(const QPixmap &pixmap);

signals:
	void clicked();

protected:
	void mouseReleaseEvent(QMouseEvent *e);
};


class SlideshowPage : public Page
{
Q_OBJECT
public:
	SlideshowPage();

public slots:
	void displayImages(QList<QString> images, unsigned element);
	void startSlideshow();

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

private slots:
	void handleClose();
	void showImage(int image);
	void displayFullScreen();

private:
	QLabel *title;
	SlideshowImage *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowWindow *window;
};


class SlideshowWindow : public Window
{
Q_OBJECT
public:
	SlideshowWindow(SlideshowPage *slideshow_page);

public slots:
	void displayImages(QList<QString> images, unsigned element);
	void startSlideshow();

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

private slots:
	void handleClose();
	void showImage(int image);
	void displayNoFullScreen();

private:
	PlaybackButtons *buttons;
	SlideshowImage *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowPage *page;
};

#endif // SLIDESHOW_H
