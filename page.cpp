#include "page.h"
#include "main.h" // IMG_PATH
#include "openclient.h" // Client
#include "btbutton.h"
#include "transitionwidget.h"
#include "main_window.h"
#include "navigation_bar.h"
#include "content_widget.h"

#include <QVBoxLayout>
#include <QMetaObject> // className
#include <QTime>
#include <QDir>
#include <QApplication>


static const char *IMG_BACK = IMG_PATH "arrlf.png";

// The defines to grab screenshot of pages
#define GRAB_PAGES 0
#define SCREENSHOT_DIR "/home/bticino/cfg/extra/0"


// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;
MainWindow *Page::main_window = 0;


Page::Page(QWidget *parent) : QWidget(parent)
{
	Q_ASSERT_X(main_window, "Page::Page", "Main window not set!");
	content_widget = 0;

	// pages with parent have a special meaning (for example, sound diffusion)
	// so they must not handled here
	if (!parent)
		main_window->addPage(this);
}

void Page::buildPage(ContentWidget *content, NavigationBar *nav_bar, QWidget *top_widget)
{
	QBoxLayout *l = new QVBoxLayout(this);
	// the top_widget (if present) is a widget that must be at the top of the page,
	// limiting the height (so even the navigation) of the ContentWidget
	if (top_widget)
		l->addWidget(top_widget);

	l->addWidget(content, 1);
	l->addWidget(nav_bar);
	l->setContentsMargins(0, 5, 0, 10);
	l->setSpacing(0);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	content_widget = content;
}

void Page::buildPage(QWidget *top_widget)
{
	buildPage(new ContentWidget, new NavigationBar, top_widget);
}

void Page::activateLayout()
{
	QLayout *main_layout = layout();
	if (main_layout)
	{
		main_layout->activate();
		main_layout->update();
	}

	if (content_widget)
		content_widget->updateLayout();
}

void Page::inizializza()
{
}

void Page::setMainWindow(MainWindow *window)
{
	main_window = window;
}

Page *Page::currentPage()
{
	return main_window->currentPage();
}

void Page::setCurrentPage()
{
	main_window->setCurrentPage(this);
}

void Page::showPage()
{
	main_window->showPage(this);

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
	Q_ASSERT_X(client_comandi, "Page::sendFrame", "Client comandi not set!");
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void Page::sendInit(QString frame) const
{
	Q_ASSERT_X(client_richieste, "Page::sendInit", "Client richieste not set!");
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

void Page::startTransition(const QPixmap &prev_image)
{
	main_window->startTransition(prev_image, this);
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


