#include "slideshow.h"
#include "navigation_bar.h"
#include "multimedia_buttons.h"
#include "displaycontrol.h" // forceOperativeMode

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QTimer>

#define SLIDESHOW_TIMEOUT 10000
#define BUTTONS_TIMEOUT 5000


// SlideshowController implementation

SlideshowController::SlideshowController(QObject *parent)
	: QObject(parent),
	timer(this)
{
	connect(&timer, SIGNAL(timeout()), SLOT(nextImageSlideshow()));
}

void SlideshowController::initialize(int total, int current)
{
	total_images = total;
	current_image = current;
}

void SlideshowController::prevImageUser()
{
	// if the slideshow timer is active, restart it after changing current image
	bool active = slideshowActive();
	if (active)
		timer.stop();

	current_image -= 1;
	if (current_image < 0)
		current_image = total_images - 1;
	emit showImage(current_image);

	if (active)
		timer.start(SLIDESHOW_TIMEOUT);
}

void SlideshowController::nextImageUser()
{
	// if the slideshow timer is active, restart it after changing current image
	bool active = slideshowActive();
	if (active)
		timer.stop();

	nextImageSlideshow();

	if (active)
		timer.start(SLIDESHOW_TIMEOUT);
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
	if (slideshowActive())
		return;
	timer.start(SLIDESHOW_TIMEOUT);
	bt_global::display.forceOperativeMode(true);
	emit slideshowStarted();
}

void SlideshowController::stopSlideshow()
{
	if (!slideshowActive())
		return;
	timer.stop();
	bt_global::display.forceOperativeMode(false);
	emit slideshowStopped();
}

bool SlideshowController::slideshowActive()
{
	return timer.isActive();
}

int SlideshowController::currentImage()
{
	return current_image;
}


// SlideshowImage implementation

SlideshowImage::SlideshowImage()
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setAlignment(Qt::AlignCenter);
}

void SlideshowImage::setPixmap(const QPixmap &pixmap)
{
	QSize screen_size = size(), pixmap_size = pixmap.size();
	QPixmap scaled = pixmap;

	// resize the pixmap if it's too big for the screen
	if (pixmap_size.width() > screen_size.width() ||
	    pixmap_size.height() > screen_size.height())
		scaled = pixmap.scaled(screen_size, Qt::KeepAspectRatio);

	QLabel::setPixmap(scaled);
}

void SlideshowImage::mouseReleaseEvent(QMouseEvent *e)
{
	QLabel::mouseReleaseEvent(e);

	emit clicked();
}


// SlideshowPage implementation

SlideshowPage::SlideshowPage()
{
	controller = new SlideshowController(this);
	window = new SlideshowWindow(this);

	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);

	// file name
	title = new QLabel;

	// pixmap used to display the image
	image = new SlideshowImage;

	MultimediaPlayerButtons *buttons = new MultimediaPlayerButtons(MultimediaPlayerButtons::IMAGE_PAGE);

	l->addWidget(title, 0, Qt::AlignHCenter);
	l->addWidget(image, 1);
	l->addWidget(buttons, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar(QString(), QString(), QString(), "back");
	buildPage(content, nav_bar);

	// signals for navigation and to start/stop slideshow
	connect(buttons, SIGNAL(previous()), controller, SLOT(prevImageUser()));
	connect(buttons, SIGNAL(next()), controller, SLOT(nextImageUser()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), controller, SLOT(startSlideshow()));
	connect(buttons, SIGNAL(pause()), controller, SLOT(stopSlideshow()));
	connect(buttons, SIGNAL(fullScreen()), SLOT(displayFullScreen()));

	// update the icon of the play button
	connect(controller, SIGNAL(slideshowStarted()), buttons, SLOT(started()));
	connect(controller, SIGNAL(slideshowStopped()), buttons, SLOT(stopped()));

	connect(controller, SIGNAL(showImage(int)), SLOT(showImage(int)));
	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));

	// close the slideshow page when the user clicks the stop button on the
	// full screen slide show
	connect(window, SIGNAL(Closed()), SLOT(handleClose()));
}

void SlideshowPage::displayImages(QList<QString> images, unsigned element)
{
	controller->initialize(images.size(), element);
	image_list = images;
	showImage(element);
	showPage();
}

void SlideshowPage::showImage(int index)
{
	image->setPixmap(image_list[index]);
	title->setText(QFileInfo(image_list[index]).fileName());
}

void SlideshowPage::startSlideshow()
{
	controller->startSlideshow();
}

void SlideshowPage::handleClose()
{
	controller->stopSlideshow();
	emit Closed();
}

void SlideshowPage::hideEvent(QHideEvent *event)
{
	controller->stopSlideshow();
}

void SlideshowPage::displayFullScreen()
{
	bool active = controller->slideshowActive();
	controller->stopSlideshow();
	window->displayImages(image_list, controller->currentImage());
	if (active)
		window->startSlideshow();
}


// SlideshowWindow implementation

SlideshowWindow::SlideshowWindow(SlideshowPage *slideshow_page)
	: buttons_timer(this)
{
	controller = new SlideshowController(this);
	page = slideshow_page;

	// pixmap used to display the image
	image = new SlideshowImage;

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
	connect(buttons, SIGNAL(previous()), controller, SLOT(prevImageUser()));
	connect(buttons, SIGNAL(next()), controller, SLOT(nextImageUser()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), controller, SLOT(startSlideshow()));
	connect(buttons, SIGNAL(pause()), controller, SLOT(stopSlideshow()));
	connect(buttons, SIGNAL(noFullScreen()), SLOT(displayNoFullScreen()));

	// update the icon of the play button
	connect(controller, SIGNAL(slideshowStarted()), buttons, SLOT(started()));
	connect(controller, SIGNAL(slideshowStopped()), buttons, SLOT(stopped()));

	connect(controller, SIGNAL(showImage(int)), this, SLOT(showImage(int)));

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
}

void SlideshowWindow::displayImages(QList<QString> images, unsigned element)
{
	controller->initialize(images.size(), element);
	image_list = images;
	showImage(element);
	showWindow();
}

void SlideshowWindow::showImage(int index)
{
	image->setPixmap(image_list[index]);
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
	emit Closed();
}

void SlideshowWindow::hideEvent(QHideEvent *event)
{
	controller->stopSlideshow();
}

void SlideshowWindow::displayNoFullScreen()
{
	bool active = controller->slideshowActive();
	controller->stopSlideshow();
	page->displayImages(image_list, controller->currentImage());
	if (active)
		page->startSlideshow();
}
