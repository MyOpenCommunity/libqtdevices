#include "page.h"
#include "main.h" // IMG_PATH
#include "openclient.h" // Client
#include "btbutton.h"
#include "transitionwidget.h"
#include "pagecontainer.h"
#include "navigation_bar.h"
#include "bannercontent.h"
#include "hardware_functions.h" // hardwareType()
#include "fontmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMetaObject> // className
#include <QTime>
#include <QDir>
#include <QApplication>
#include <QLabel>


static const char *IMG_BACK = IMG_PATH "arrlf.png";

// The defines to grab screenshot of pages
#define GRAB_PAGES 0
#define SCREENSHOT_DIR "/home/bticino/cfg/extra/0"


// Inizialization of static member
Client *Page::client_comandi = 0;
Client *Page::client_richieste = 0;
PageContainer *Page::page_container = 0;


PageTitleWidget::PageTitleWidget(const QString &label, int height)
{
	QLabel *title = new QLabel(label);
	title->setFont(bt_global::font->get(FontManager::TITLE));
	title->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	title->setFixedHeight(height);

	QLabel *current_page = new QLabel;
	current_page->setFont(bt_global::font->get(FontManager::TEXT));
	current_page->setAlignment(Qt::AlignTop|Qt::AlignRight);

	QHBoxLayout *t = new QHBoxLayout(this);
	t->setContentsMargins(10, 0, 10, 10);
	t->addStretch(1);
	t->addWidget(title);
	t->addWidget(current_page, 1);
}

void PageTitleWidget::setTitle(QString title)
{
	QLabel *label = static_cast<QLabel*>(layout()->itemAt(1)->widget());

	label->setText(title);
}

void PageTitleWidget::setCurrentPage(int current, int total)
{
	if (total == 1)
		return;

	QLabel *label = static_cast<QLabel*>(layout()->itemAt(2)->widget());

	label->setText(QString("%1/%2").arg(current + 1).arg(total));
}


Page::Page(QWidget *parent) : StyledWidget(parent)
{
	Q_ASSERT_X(page_container, "Page::Page", "PageContainer not set!");
	__content = 0;

	// pages with parent have a special meaning (for example, sound diffusion)
	// so they must not handled here
	if (!parent)
		page_container->addPage(this);
}

void Page::buildPage(QWidget *content, QWidget *nav_bar, const QString& label, int label_height, QWidget *top_widget)
{
	QLabel *page_title = 0;

	if (hardwareType() == TOUCH_X)
	{
		page_title = new QLabel(label);
		page_title->setFont(bt_global::font->get(FontManager::TITLE));
		page_title->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
		page_title->setFixedHeight(label_height);
	}

	buildPage(content, nav_bar, top_widget, page_title);
}

void Page::buildPage(QWidget *content, QWidget *nav_bar, QWidget *top_widget, QWidget *title_widget)
{
	QBoxLayout *l;
	if (hardwareType() == TOUCH_X)
		l = new QHBoxLayout(this);
	else
		l = new QVBoxLayout(this);

	// the top_widget (if present) is a widget that must be at the top of the page,
	// limiting the height (so even the navigation) of the content; for TouchX
	// it must be between the title and the content

	if (hardwareType() == TOUCH_X)
	{
		// TODO add an API to set the page title and to set the page count
		//      and current page number
		l->addWidget(nav_bar, 0);

		QVBoxLayout *pl = new QVBoxLayout;
		pl->setContentsMargins(0, 0, 0, 0);
		pl->setSpacing(0);
		if (title_widget)
			pl->addWidget(title_widget);
		if (top_widget)
			pl->addWidget(top_widget);
		pl->addWidget(content, 1);

		l->addLayout(pl, 1);
	}
	else
	{
		if (top_widget)
			l->addWidget(top_widget);

		Q_ASSERT_X(title_widget == NULL, "Page::buildPage",
			   "BTouch pages can't have a title");
		l->addWidget(content, 1);
		l->addWidget(nav_bar);
	}
	l->setContentsMargins(0, 5, 0, 10);
	l->setSpacing(0);

	__content = content;
}

void Page::activateLayout()
{
	QLayout *main_layout = layout();
	if (main_layout)
	{
		main_layout->activate();
		main_layout->update();
	}
}

void Page::inizializza()
{
}

void Page::setPageContainer(PageContainer *container)
{
	page_container = container;
}

Page *Page::currentPage()
{
	return page_container->currentPage();
}

void Page::setCurrentPage()
{
	page_container->setCurrentPage(this);
}

void Page::showPage()
{
	page_container->showPage(this);

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

void Page::prepareTransition()
{
	page_container->prepareTransition();
}

void Page::startTransition()
{
	page_container->startTransition(this);
}

Page::PageType Page::pageType()
{
	return NONE;
}

int Page::sectionId()
{
	return NO_SECTION;
}


BannerPage::BannerPage(QWidget *parent)
	: Page(parent)
{
}

void BannerPage::inizializza()
{
	if (page_content)
		page_content->initBanners();
}

void BannerPage::activateLayout()
{
	if (page_content)
		page_content->updateGeometry();

	Page::activateLayout();

	if (page_content)
		page_content->drawContent();
}

void BannerPage::buildPage(BannerContent *content, NavigationBar *nav_bar, const QString &title, QWidget *top_widget)
{
	buildPage(content, content, nav_bar, title, TITLE_HEIGHT, top_widget);
}

void BannerPage::buildPage(QWidget *parent, BannerContent *content, NavigationBar *nav_bar,
			   const QString &title, int title_height, QWidget *top_widget)
{
	if (hardwareType() == TOUCH_X)
	{
		PageTitleWidget *title_widget = new PageTitleWidget(title, title_height);
		Page::buildPage(parent, nav_bar, top_widget, title_widget);

		connect(content, SIGNAL(contentScrolled(int, int)),
			title_widget, SLOT(setCurrentPage(int, int)));
	}
	else
		Page::buildPage(parent, nav_bar, top_widget);
	__content = content;

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

void BannerPage::buildPage(const QString &title, QWidget *top_widget)
{
	buildPage(new BannerContent, new NavigationBar, title, top_widget);
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


