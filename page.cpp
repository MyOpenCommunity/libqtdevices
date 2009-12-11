#include "page.h"
#include "main.h" // MAX_WIDTH, MAX_HEIGHT, IMG_PATH
#include "openclient.h" // Client
#include "btbutton.h"
#include "transitionwidget.h"
#include "navigation_bar.h"
#include "bannercontent.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QMetaObject> // className
#include <QTime>
#include <QDir>

#include <assert.h>

static const char *IMG_BACK = IMG_PATH "arrlf.png";

// The defines to grab screenshot of pages
#define GRAB_PAGES 0
#define SCREENSHOT_DIR "/home/bticino/cfg/extra/0"


// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;
QStackedWidget *Page::main_window = 0;
TransitionWidget *Page::transition_widget = 0;
bool Page::block_transitions = false;


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

void Page::blockTransitions(bool block)
{
	block_transitions = block;
	if (block && transition_widget)
		transition_widget->cancelTransition();
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
		transition_widget->setStartingPage(static_cast<Page *>(main_window->currentWidget()));
}

void Page::startTransition()
{
	if (transition_widget)
		transition_widget->startTransition(this);
}

Page *Page::currentPage()
{
	Page *curr = static_cast<Page*>(main_window->currentWidget());

	// if we are in the middle of a transition, we use the previous page as the current page
	if (TransitionWidget *t = qobject_cast<TransitionWidget*>(curr))
		return t->prev_page;

	return curr;
}

void Page::showPage()
{
	qDebug() << "Page::showPage on" << this;
	if (transition_widget && !block_transitions)
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

BannerPage::BannerPage(QWidget *parent)
	: Page(parent)
{
}

void BannerPage::activateLayout()
{
	if (page_content)
		page_content->updateGeometry();

	QLayout *main_layout = layout();
	if (main_layout)
	{
		main_layout->activate();
		main_layout->update();
	}

	if (page_content)
		page_content->drawContent();
}

void BannerPage::buildPage(BannerContent *content, NavigationBar *nav_bar, QWidget *top_widget)
{
	QBoxLayout *l = new QVBoxLayout(this);

	// the top_widget (if present) is a widget that must be at the top of the page,
	// limiting the height (so even the navigation) of the content
	if (top_widget)
		l->addWidget(top_widget);

	l->addWidget(content, 1);
	l->addWidget(nav_bar);

	l->setContentsMargins(0, 5, 0, 10);
	l->setSpacing(0);

	__content = content;

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

void BannerPage::buildPage(QWidget *top_widget)
{
	buildPage(new BannerContent, new NavigationBar, top_widget);
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

