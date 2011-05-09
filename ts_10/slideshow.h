/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include "page.h"
#include "window.h"

#include <QList>
#include <QTimer>
#include <QFutureWatcher>
#include <QPointer>

class QLabel;
class ImageLabel;
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

	// start/stop the slideshow. During it, the display mode is forced to be operative.
	void startSlideshow();
	void stopSlideshow();

	void startImageTimer();

signals:
	// emitted when the slideshow is starting/stopping
	void slideshowStarted();
	void slideshowStopped();

	// emitted when the current image changes
	void showImage(int);

private:
	int total_images, current_image;
	bool active;
	QTimer timer;

};


/*!
	\ingroup Multimedia
	\brief Display images in a slide show (either manual or automatic).

	\see SlideshowWindow
 */
class SlideshowPage : public Page
{
Q_OBJECT
public:
	SlideshowPage();
	~SlideshowPage();

public slots:
	// displays the page and stores the image list for the slide show
	void displayImages(QList<QString> images, unsigned element);

	// starts the slide show from the current image
	void startSlideshow();

	virtual void cleanUp();

signals:
	void cleanedUp();
	void imageLoaded();
	void imageNotLoaded();

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

	void showEvent(QShowEvent *event);

private slots:
	void handleClose();
	void showImage(int image);
	void displayFullScreen();
	void imageReady();
	void loadImage();

private:
	bool goto_fullscreen;
	QString image_to_load;
	QLabel *title;
	ImageLabel *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowWindow *window;
	QPointer<QFutureWatcher<QImage> > async_load;
	// when we exit from the page due to a videocall or an alarm we want to paused
	// and then restore the slideshow.
	bool paused;
};


/*!
	\ingroup Multimedia
	\brief Display images full-screen in a slide show (either manual or automatic.

	\see SlideshowPage
 */
class SlideshowWindow : public Window
{
Q_OBJECT
public:
	SlideshowWindow(SlideshowPage *slideshow_page);
	~SlideshowWindow();

	virtual void showWindow();

public slots:
	// displays the page and stores the image list for the slide show
	void displayImages(QList<QString> images, unsigned element);

	// starts the slide show from the current image
	void startSlideshow();

signals:
	void imageLoaded();
	void imageNotLoaded();
	void closePage();

protected:
	// kills the slideshow timer
	void hideEvent(QHideEvent *event);

	void showEvent(QShowEvent *event);

private slots:
	void handleClose();
	void showImage(int image);
	void displayNoFullScreen();
	void showButtons();
	void imageReady();
	void loadImage();

private:
	bool goto_normalscreen;
	QString image_to_load;
	// used to automatically hide the buttons
	QTimer buttons_timer;
	MultimediaPlayerButtons *buttons;
	ImageLabel *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowPage *page;
	QPointer<QFutureWatcher<QImage> > async_load;
	bool paused;
};

#endif // SLIDESHOW_H
