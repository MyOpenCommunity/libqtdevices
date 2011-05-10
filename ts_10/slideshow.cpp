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


#include "slideshow.h"
#include "navigation_bar.h"
#include "multimedia_buttons.h"
#include "displaycontrol.h" // forceOperativeMode
#include "labels.h" // ImageLabel, WaitLabel
#include "pagestack.h"
#include "generic_functions.h" // checkImageSize, checkImageMemory, startTrackMemory, stopTrackMemory
#include "hardware_functions.h" // dumpSystemMemory
#include "fontmanager.h" // bt_global::font

#include <QImageReader>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QTimer>

#include <QtConcurrentRun>

#define SLIDESHOW_TIMEOUT 10000
#define BUTTONS_TIMEOUT 5000
#define IMAGELOAD_MSEC_WAIT_TIME 300

namespace
{
	QImage buildImage(const QString &image)
	{
		return loadImage(image);
	}
}

// SlideshowController implementation

SlideshowController::SlideshowController(QObject *parent)
	: QObject(parent),
	timer(this)
{
	timer.setSingleShot(true);
	timer.setInterval(SLIDESHOW_TIMEOUT);
	connect(&timer, SIGNAL(timeout()), SLOT(nextImageSlideshow()));
}

void SlideshowController::initialize(int total, int current)
{
	total_images = total;
	current_image = current;
}

void SlideshowController::startImageTimer()
{
	if (active)
		timer.start();
}

void SlideshowController::prevImageUser()
{
	// if the slideshow timer is active, restart it after changing current image
	if (active && timer.isActive())
		timer.stop();

	current_image -= 1;
	if (current_image < 0)
		current_image = total_images - 1;
	emit showImage(current_image);
}

void SlideshowController::nextImageUser()
{
	// if the slideshow timer is active, restart it after changing current image
	if (active && timer.isActive())
		timer.stop();

	nextImageSlideshow();
}

void SlideshowController::nextImageSlideshow()
{
	current_image += 1;
	if (current_image >= total_images)
		current_image = 0;
	emit showImage(current_image);
}

void SlideshowController::startSlideshow()
{
	if (active)
		return;
	active = true;
	timer.start();
	bt_global::display->forceOperativeMode(true);
	emit slideshowStarted();
}

void SlideshowController::stopSlideshow()
{
	if (!active)
		return;
	if (timer.isActive())
		timer.stop();
	active = false;
	bt_global::display->forceOperativeMode(false);
	emit slideshowStopped();
}

bool SlideshowController::slideshowActive()
{
	return active;
}

int SlideshowController::currentImage()
{
	return current_image;
}


// SlideshowPage implementation

SlideshowPage::SlideshowPage()
{
	controller = new SlideshowController(this);
	window = new SlideshowWindow(this);
	paused = false;

	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);

	// file name
	title = new QLabel;

	// pixmap used to display the image
	image = new ImageLabel;
	image->setFont(bt_global::font->get(FontManager::TEXT));

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::IMAGE_PAGE);

	l->addWidget(title, 0, Qt::AlignHCenter);
	l->addWidget(image, 1);
	l->addWidget(buttons, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar(QString(), QString(), QString(), "back");
	buildPage(content, nav_bar);

	// signals for navigation and to start/stop slideshow
	connect(buttons, SIGNAL(previous()), SLOT(prevImageUser()));
	connect(buttons, SIGNAL(next()), SLOT(nextImageUser()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), controller, SLOT(startSlideshow()));
	connect(buttons, SIGNAL(pause()), controller, SLOT(stopSlideshow()));
	connect(buttons, SIGNAL(fullScreen()), SLOT(displayFullScreen()));

	// update the icon of the play button
	connect(controller, SIGNAL(slideshowStarted()), buttons, SLOT(started()));
	connect(controller, SIGNAL(slideshowStopped()), buttons, SLOT(stopped()));

	connect(controller, SIGNAL(showImage(int)), SLOT(showImage(int)));
	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));

	// To automatically get the next image after the previuos one is loaded.
	connect(this, SIGNAL(imageLoaded()), controller, SLOT(startImageTimer()));
	connect(this, SIGNAL(imageNotLoaded()), controller, SLOT(startImageTimer()));

	// close the slideshow page when the user clicks the stop button on the
	// full screen slide show
	connect(window, SIGNAL(closePage()), SLOT(handleClose()));

	working = 0;
	show_working = false;
}

SlideshowPage::~SlideshowPage()
{
	if (async_load)
		async_load->deleteLater();
}

void SlideshowPage::prevImageUser()
{
	show_working = true;
	controller->prevImageUser();
}

void SlideshowPage::nextImageUser()
{
	show_working = true;
	controller->nextImageUser();
}

void SlideshowPage::displayImages(QList<QString> images, unsigned element)
{
	controller->initialize(images.size(), element);
	image_list = images;
	showPixmap(QPixmap(), QString());
	showImage(element);
	showPage();
}

void SlideshowPage::showPixmap(const QPixmap &pixmap, const QString &text_title)
{
	image->setStyleSheet("");
	image->setText(QString());
	image->setPixmap(pixmap);
	title->setText(text_title);
}

void SlideshowPage::showMessage(const QString &text, const QString &text_title)
{
	image->setPixmap(QPixmap());
	image->setStyleSheet("background: black");
	image->setText(text);
	title->setText(text_title);
}

void SlideshowPage::showImage(int index)
{
	QString image = image_list[index];

	if (!checkImageSize(image))
	{
		qDebug() << "The image" << image << "exceed the maximum size";
		showMessage(tr("Image not supported"), QFileInfo(image).fileName());
		emit imageNotLoaded();
		return;
	}

	if (!checkImageMemory(image))
	{
		qDebug() << "Unable to load the image" << image;
		showMessage(tr("Image exceed memory size"), QFileInfo(image).fileName());
		emit imageNotLoaded();
		return;
	}

#if TRACK_IMAGES_MEMORY
	startTrackMemory();
#endif

	qDebug() << "Loading image" << image << this;

	async_load = new QFutureWatcher<QImage>();
	connect(async_load, SIGNAL(finished()), SLOT(imageReady()));

	async_load->setFuture(QtConcurrent::run(&buildImage, image));
	if (show_working)
	{
		Q_ASSERT_X(working == 0, "SlideshowPage::loadImage", "Multiple operations in progress");
		working = new WaitLabel(this, IMAGELOAD_MSEC_WAIT_TIME);
		connect(this, SIGNAL(Closed()), working, SLOT(abort()));
		show_working = false;
	}
}

void SlideshowPage::imageReady()
{
	if (working)
	{
		working->waitForTimeout();
		working = 0;
	}

	qDebug() << "Image loading complete" << this;
	emit imageLoaded();

	showPixmap(QPixmap::fromImage(async_load->result()), QFileInfo(image_list[controller->currentImage()]).fileName());
	async_load->disconnect();
	async_load->deleteLater();

#if TRACK_IMAGES_MEMORY
	stopTrackMemory();
#endif
}

void SlideshowPage::startSlideshow()
{
	controller->startSlideshow();
}

void SlideshowPage::handleClose()
{
	qDebug() << "SlideshowPage::handleClose";
	controller->stopSlideshow();
	emit Closed();
}

void SlideshowPage::cleanUp()
{
	// We want that the slideshow is paused/restarted when we exit from the page
	// due to a videocall or an alarm, but not when we exit due to a click from
	// an user. The cleanUp method is used to avoid a restart when the user
	// clicks on the heeder widget and it works only because the cleanUp is
	// performed before closing the page (so also before the hideEvent).
	controller->stopSlideshow();
	emit cleanedUp();
}

void SlideshowPage::hideEvent(QHideEvent *)
{
	if (controller->slideshowActive())
	{
		controller->stopSlideshow();
		paused = true;
	}
}

void SlideshowPage::showEvent(QShowEvent *)
{
	if (paused)
		controller->startSlideshow();
	paused = false;
}

void SlideshowPage::displayFullScreen()
{
	bool active = controller->slideshowActive();
	controller->stopSlideshow();
	window->displayImages(image_list, controller->currentImage());

	if (async_load)
	{
		async_load->disconnect();
		async_load->deleteLater();
	}

	if (active)
		window->startSlideshow();
}


// SlideshowWindow implementation

SlideshowWindow::SlideshowWindow(SlideshowPage *slideshow_page)
	: buttons_timer(this)
{
	controller = new SlideshowController(this);
	page = slideshow_page;
	paused = false;

	// pixmap used to display the image
	image = new ImageLabel;
	image->setFont(bt_global::font->get(FontManager::TEXT));

	buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::IMAGE_WINDOW);
	buttons->hide();

	QGridLayout *button_layout = new QGridLayout(image);
	button_layout->setContentsMargins(0, 0, 0, 0);
	button_layout->setColumnStretch(0, 1);
	button_layout->setColumnStretch(2, 1);
	button_layout->setRowStretch(0, 1);
	button_layout->addWidget(buttons, 1, 1);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->addWidget(image, 1);

	// signals for navigation and to start/stop slideshow
	connect(buttons, SIGNAL(previous()), SLOT(prevImageUser()));
	connect(buttons, SIGNAL(next()), SLOT(nextImageUser()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), controller, SLOT(startSlideshow()));
	connect(buttons, SIGNAL(pause()), controller, SLOT(stopSlideshow()));
	connect(buttons, SIGNAL(noFullScreen()), SLOT(displayNoFullScreen()));

	// update the icon of the play button
	connect(controller, SIGNAL(slideshowStarted()), buttons, SLOT(started()));
	connect(controller, SIGNAL(slideshowStopped()), buttons, SLOT(stopped()));

	connect(controller, SIGNAL(showImage(int)), this, SLOT(showImage(int)));

	// To automatically get the next image after the previuos one is loaded.
	connect(this, SIGNAL(imageLoaded()), controller, SLOT(startImageTimer()));
	connect(this, SIGNAL(imageNotLoaded()), controller, SLOT(startImageTimer()));

	// timer to hide the buttons
	buttons_timer.setSingleShot(true);
	connect(&buttons_timer, SIGNAL(timeout()), buttons, SLOT(hide()));

	// this shows the buttons when the user clicks the screen
	connect(image, SIGNAL(clicked()), SLOT(showButtons()));

	// these reset the buttons timer on each click
	connect(buttons, SIGNAL(previous()), SLOT(showButtons()));
	connect(buttons, SIGNAL(next()), SLOT(showButtons()));
	connect(buttons, SIGNAL(play()), SLOT(showButtons()));
	connect(buttons, SIGNAL(pause()), SLOT(showButtons()));

	working = 0;
	show_working = false;
}

void SlideshowWindow::prevImageUser()
{
	show_working = true;
	controller->prevImageUser();
}

void SlideshowWindow::nextImageUser()
{
	show_working = true;
	controller->nextImageUser();
}

SlideshowWindow::~SlideshowWindow()
{
	if (async_load)
		async_load->deleteLater();
}

void SlideshowWindow::showWindow()
{
	// showUserWindow is required when we exit from the screensaver, because we
	// want to turn back to the window (otherwise, we turn back to the SlideshowPage)
	bt_global::page_stack.showUserWindow(this);
	Window::showWindow();
}

void SlideshowWindow::displayImages(QList<QString> images, unsigned element)
{
	controller->initialize(images.size(), element);
	image_list = images;
	showPixmap(QPixmap());
	showImage(element);
	showWindow();
}

void SlideshowWindow::showPixmap(const QPixmap &pixmap)
{
	image->setStyleSheet("");
	image->setText(QString());
	image->setPixmap(pixmap);
}

void SlideshowWindow::showMessage(const QString &text)
{
	image->setPixmap(QPixmap());
	image->setStyleSheet("background: black");
	image->setText(text);
}

void SlideshowWindow::showImage(int index)
{
	QString image = image_list[index];

	if (!checkImageSize(image))
	{
		qDebug() << "The image" << image << "exceed the maximum size";
		showMessage(tr("Image not supported"));
		emit imageNotLoaded();
		return;
	}

	if (!checkImageMemory(image))
	{
		qDebug() << "Unable to load the image" << image;
		showMessage(tr("Image exceed memory size"));
		emit imageNotLoaded();
		return;
	}

#if TRACK_IMAGES_MEMORY
	startTrackMemory();
#endif

	qDebug() << "Loading image" << image;

	async_load = new QFutureWatcher<QImage>();
	connect(async_load, SIGNAL(finished()), SLOT(imageReady()));
	async_load->setFuture(QtConcurrent::run(&buildImage, image));

	if (show_working)
	{
		Q_ASSERT_X(working == 0, "SlideshowPage::loadImage", "Multiple operations in progress");
		working = new WaitLabel(this, IMAGELOAD_MSEC_WAIT_TIME);
		connect(this, SIGNAL(Closed()), working, SLOT(abort()));
		show_working = false;
	}
}

void SlideshowWindow::imageReady()
{
	if (working)
	{
		working->waitForTimeout();
		working = 0;
	}

	qDebug() << "Image loading complete";
	emit imageLoaded();

	showPixmap(QPixmap::fromImage(async_load->result()));
	async_load->disconnect();
	async_load->deleteLater();

#if TRACK_IMAGES_MEMORY
	stopTrackMemory();
#endif
}

void SlideshowWindow::showButtons()
{
	buttons_timer.stop();
	buttons->show();
	buttons_timer.start(BUTTONS_TIMEOUT);
}

void SlideshowWindow::startSlideshow()
{
	controller->startSlideshow();
}

void SlideshowWindow::handleClose()
{
	controller->stopSlideshow();
	// We cannot simply use the Closed signal to close the window, because the signal
	// is also used to remove the Window from the stack (so, we can show the wrong
	// page or trigger an assert when calling the fileselector::showPage twice).
	// We use a different signal and manually remove the Window from the stack
	// to avoid the problem.
	bt_global::page_stack.closeWindow(this);
	emit closePage();
}

void SlideshowWindow::hideEvent(QHideEvent *)
{
	if (controller->slideshowActive())
	{
		controller->stopSlideshow();
		paused = true;
	}
}

void SlideshowWindow::showEvent(QShowEvent *)
{
	if (paused)
		controller->startSlideshow();
	paused = false;
}

void SlideshowWindow::displayNoFullScreen()
{
	bool active = controller->slideshowActive();
	controller->stopSlideshow();
	page->displayImages(image_list, controller->currentImage());
	// We have to remove the window from the stack, automatically done when
	// we close using the signal Closed()
	bt_global::page_stack.closeWindow(this);

	if (active)
		page->startSlideshow();
}
