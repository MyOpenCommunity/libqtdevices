#include "slideshow.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "navigation_bar.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QTimer>

#define SLIDESHOW_TIMEOUT 10000


SlideshowController::SlideshowController(QObject *parent)
	: QObject(parent)
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(nextImageSlideshow()));
}

void SlideshowController::initialize(int total, int current)
{
	total_images = total;
	current_image = current;
}

void SlideshowController::prevImageUser()
{
	bool active = slideshowActive();
	if (active)
		timer->stop();

	current_image -= 1;
	if (current_image < 0)
		current_image = total_images - 1;
	emit showImage(current_image);

	if (active)
		timer->start(SLIDESHOW_TIMEOUT);
}

void SlideshowController::nextImageUser()
{
	bool active = slideshowActive();
	if (active)
		timer->stop();

	nextImageSlideshow();

	if (active)
		timer->start(SLIDESHOW_TIMEOUT);
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
	timer->start(SLIDESHOW_TIMEOUT);
	emit slideshowStarted();
}

void SlideshowController::stopSlideshow()
{
	if (!slideshowActive())
		return;
	timer->stop();
	emit slideshowStopped();
}

bool SlideshowController::slideshowActive()
{
	return timer->isActive();
}


static inline BtButton *getButton(const QString &icon)
{
	BtButton *button = new BtButton;
	button->setImage(icon);

	return button;
}

PlaybackButtons::PlaybackButtons(Type type)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(type == IN_WINDOW ? 0 : 5);

	play_icon = bt_global::skin->getImage("start");
	pause_icon = bt_global::skin->getImage("pause");

	BtButton *prev = getButton(bt_global::skin->getImage("previous"));
	BtButton *next = getButton(bt_global::skin->getImage("next"));
	BtButton *stop = getButton(bt_global::skin->getImage("stop"));
	BtButton *screen = getButton(bt_global::skin->getImage(type == IN_WINDOW ? "nofullscreen" : "fullscreen"));

	play_button = getButton(play_icon);
	play_button->setPressedImage(pause_icon);
	play_button->setCheckable(true);
	play_button->setOnOff();

	l->addWidget(prev);
	l->addWidget(play_button);
	l->addWidget(stop);
	l->addWidget(next);
	l->addWidget(screen);

	connect(play_button, SIGNAL(toggled(bool)), SLOT(playToggled(bool)));
	connect(prev, SIGNAL(clicked()), SIGNAL(previous()));
	connect(next, SIGNAL(clicked()), SIGNAL(next()));
	connect(stop, SIGNAL(clicked()), SIGNAL(stop()));
	connect(screen, SIGNAL(clicked()), type == IN_PAGE ? SIGNAL(fullScreen()) : SIGNAL(noFullScreen()));
}

void PlaybackButtons::playToggled(bool playing)
{
	if (playing)
		emit play();
	else
		emit pause();
}

void PlaybackButtons::started()
{
	play_button->setStatus(true);
}

void PlaybackButtons::stopped()
{
	play_button->setStatus(false);
}


SlideshowPage::SlideshowPage()
{
	controller = new SlideshowController(this);

	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);

	// file name
	title = new QLabel("File name here");

	// pixmap used to display the image
	image = new QLabel;
	image->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	image->setAlignment(Qt::AlignCenter);

	PlaybackButtons *buttons = new PlaybackButtons(PlaybackButtons::IN_PAGE);

	l->addWidget(title, 0, Qt::AlignHCenter);
	l->addWidget(image, 1);
	l->addWidget(buttons, 0, Qt::AlignHCenter);

	NavigationBar *nav_bar = new NavigationBar(QString(), QString(), QString(), "back");
	buildPage(content, nav_bar);

	connect(buttons, SIGNAL(previous()), controller, SLOT(prevImageUser()));
	connect(buttons, SIGNAL(next()), controller, SLOT(nextImageUser()));
	connect(buttons, SIGNAL(stop()), SLOT(handleClose()));
	connect(buttons, SIGNAL(play()), controller, SLOT(startSlideshow()));
	connect(buttons, SIGNAL(pause()), controller, SLOT(stopSlideshow()));

	connect(controller, SIGNAL(slideshowStarted()), buttons, SLOT(started()));
	connect(controller, SIGNAL(slideshowStopped()), buttons, SLOT(stopped()));
	connect(controller, SIGNAL(showImage(int)), this, SLOT(showImage(int)));

	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));
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
	QPixmap pixmap(image_list[index]);
	QSize screen_size = image->size(), pixmap_size = pixmap.size();

	// resize the pixmap if it's too big for the screen
	if (pixmap_size.width() > screen_size.width() ||
	    pixmap_size.height() > screen_size.height())
		pixmap = pixmap.scaled(screen_size, Qt::KeepAspectRatio);

	image->setPixmap(pixmap);
	title->setText(QFileInfo(image_list[index]).fileName());
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


