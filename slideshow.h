#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "page.h"
#include "window.h"

#include <QList>
#include <QLabel>

class QLabel;
class QTimer;
class SlideshowWindow;
class MultimediaPlayerButtons;


// helper class, handles manual navigation over an item list,
// with an optional slide show mode
class SlideshowController : public QObject
{
Q_OBJECT
public:
	SlideshowController(QObject *parent);

	void initialize(int total, int current);
	bool slideshowActive();
	int currentImage();

public slots:
	// call when the user clicks on the "previous" button
	void prevImageUser();

	// call when the user clicks on the "next" button
	void nextImageUser();

	// called internally by the slideshow timer
	void nextImageSlideshow();

	// start/stop the slideshow
	void startSlideshow();
	void stopSlideshow();

signals:
	// emitted when the slideshow is starting/stopping
	void slideshowStarted();
	void slideshowStopped();

	// emitted when the current image changes
	void showImage(int);

private:
	int total_images, current_image;
	QTimer *timer;
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
	// displays the page and stores the image list for the slide show
	void displayImages(QList<QString> images, unsigned element);

	// starts the slide show from the current image
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
	// displays the page and stores the image list for the slide show
	void displayImages(QList<QString> images, unsigned element);

	// starts the slide show from the current image
	void startSlideshow();

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

private slots:
	void handleClose();
	void showImage(int image);
	void displayNoFullScreen();
	void showButtons();

private:
	// used to automatically hide the buttons
	QTimer *buttons_timer;
	MultimediaPlayerButtons *buttons;
	SlideshowImage *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowPage *page;
};

#endif // SLIDESHOW_H
