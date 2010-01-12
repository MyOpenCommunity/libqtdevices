#include "iconpage.h"
#include "btbutton.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "main.h" // IMG_PATH

#include <QDomNode>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#define DIM_BUT 80
#define BACK_BUTTON_X    0
#define BACK_BUTTON_Y  250
#define BACK_BUTTON_DIM 60
#define IMG_BACK_BUTTON IMG_PATH "arrlf.png"


IconPage::IconPage()
	: buttons_group(this)
{
	connect(&buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
}

void IconPage::buildPage(IconContent *content, NavigationBar *nav_bar, const QString &title)
{
	PageTitleWidget *title_widget = 0;
	if (!title.isNull())
	{
		title_widget = new PageTitleWidget(title, 35);
		connect(content, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));
	}
	Page::buildPage(content, nav_bar, 0, title_widget);

	// TODO duplicated in BannerPage

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

void IconPage::activateLayout()
{
	if (page_content)
		page_content->updateGeometry();

	Page::activateLayout();

	if (page_content)
		page_content->drawContent();
}

BtButton *IconPage::addButton(const QString &label, const QString& icon_path, int x, int y)
{
	BtButton *b = new BtButton(this);
	if (page_content == NULL)
		b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	else
		page_content->addButton(b, label);
	b->setImage(icon_path);

	return b;
}

void IconPage::addPage(Page *page, int id, const QString &label, const QString &iconName, int x, int y)
{
	BtButton *b = addButton(label, iconName, x, y);

	buttons_group.addButton(b, id);
	page_list[id] = page;
	connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
}

void IconPage::addBackButton()
{
	BtButton *b = new BtButton(this);
	b->setGeometry(BACK_BUTTON_X, BACK_BUTTON_Y, BACK_BUTTON_DIM, BACK_BUTTON_DIM);
	b->setImage(IMG_BACK_BUTTON);
	connect(b, SIGNAL(clicked()), SIGNAL(Closed()));
}

void IconPage::clicked(int id)
{
	page_list[id]->showPage();
}


IconContent::IconContent(QWidget *parent) : GridContent(parent)
{
	QGridLayout *l = static_cast<QGridLayout *>(layout());
	l->setContentsMargins(25, 0, 25, 0);
	l->setSpacing(28);
	l->setColumnStretch(5, 1);
}

void IconContent::addButton(QWidget *button, const QString &label)
{
	QWidget *w = button;

	if (!label.isEmpty())
	{
		w = new QWidget;
		QVBoxLayout *l = new QVBoxLayout(w);
		QLabel *lbl = new QLabel(label);

		lbl->setAlignment(Qt::AlignHCenter);

		l->addWidget(button);
		l->addWidget(lbl);
	}

	items.append(w);
	w->hide();
}

void IconContent::addWidget(QWidget *widget)
{
	items.append(widget);
	widget->hide();
}

void IconContent::drawContent()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		// compute the page list
		prepareLayout(items, 4);

		// add icons to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
				l->addWidget(items.at(base + j), j / 4, j % 4);
		}

		l->setRowStretch(l->rowCount(), 1);
	}

	updateLayout(items);
}
