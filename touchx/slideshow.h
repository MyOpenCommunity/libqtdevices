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
	QTimer timer;
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
	void imageReady();

private:
	QLabel *title;
	ImageLabel *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowWindow *window;
	QFutureWatcher<QImage> async_load;
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
	void imageReady();

private:
	// used to automatically hide the buttons
	QTimer buttons_timer;
	MultimediaPlayerButtons *buttons;
	ImageLabel *image;
	QList<QString> image_list;
	SlideshowController *controller;
	SlideshowPage *page;
	QFutureWatcher<QImage> async_load;
};

#endif // SLIDESHOW_H
