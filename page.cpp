#include "page.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "openclient.h" // Client
#include "btbutton.h"
#include "transitionwidget.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QMetaObject> // className
#include <QTime>
#include <QDir>

#include <assert.h>

static const char *IMG_BACK = IMG_PATH "arrlf.png";

// The defines to grab screenshot of pages
#define GRAB_PAGES 0
#define SCREENSHOT_DIR "/tmp/screenshots"


// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;
QStackedWidget *Page::main_window = 0;
TransitionWidget *Page::transition_widget = 0;


Page::Page(QWidget *parent) : QWidget(parent)
{
	assert(main_window && "Main window not set!");

	// pages with parent have a special meaning (for example, sound diffusion)
	// so they must not handled here
	if (!parent)
		main_window->addWidget(this);
}

void Page::inizializza()
{
}

void Page::setMainWindow(QStackedWidget *window)
{
	main_window = window;
}

void Page::installTransitionWidget(TransitionWidget *tr)
{
	transition_widget = tr;
}

void Page::initTransition()
{
	if (transition_widget)
		transition_widget->setStartingImage(QPixmap::grabWidget(main_window->currentWidget()));
}

void Page::startTransition()
{
	if (transition_widget)
		transition_widget->startTransition(this);
}

Page *Page::currentPage()
{
	return static_cast<Page *>(main_window->currentWidget());
}

void Page::showPage()
{
	if (transition_widget)
	{
		initTransition();
		startTransition();
	}
	else
		main_window->setCurrentWidget(this);
#if GRAB_PAGES
	Page *p = currentPage();
	QPixmap screenshot = QPixmap::grabWidget(p);

	QDir grab_dir(SCREENSHOT_DIR);
	if (!grab_dir.exists())
		grab_dir.mkpath(SCREENSHOT_DIR);

	QString filename(QString(p->metaObject()->className()) + "_" + QTime::currentTime().toString("hh.mm.ss.zzz") + ".png");
	screenshot.save(QString(SCREENSHOT_DIR) + "/" + filename);
#endif
}

void Page::sendFrame(QString frame) const
{
	assert(client_comandi && "Client comandi not set!");
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void Page::sendInit(QString frame) const
{
	assert(client_richieste && "Client richieste not set!");
	QByteArray buf = frame.toAscii();
	client_richieste->ApriInviaFrameChiudi(buf.constData());
}

void Page::setClients(Client *command, Client *request)
{
	client_comandi = command;
	client_richieste = request;
}

void Page::forceClosed()
{
	emit Closed();
}

PageLayout::PageLayout(QWidget *parent) : Page(parent)
{
	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 0);
	main_layout->setSpacing(0);
}

void PageLayout::addBackButton()
{
	BtButton *back_btn = new BtButton;
	back_btn->setImage(IMG_BACK);
	connect(back_btn, SIGNAL(clicked()), this, SIGNAL(Closed()));
	main_layout->addStretch();
	main_layout->addWidget(back_btn, 0, Qt::AlignLeft);
	main_layout->addSpacing(10);
}

