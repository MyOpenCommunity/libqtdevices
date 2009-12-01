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
	PageTitleWidget *title_widget = new PageTitleWidget(title, 35);
	Page::buildPage(content, nav_bar, 0, title_widget);

	// TODO duplicated in BannerPage
	connect(content, SIGNAL(contentScrolled(int, int)),
		title_widget, SLOT(setCurrentPage(int, int)));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}

BtButton *IconPage::addButton(int id, const QString &label, const QString& iconName, int x, int y)
{
	BtButton *b = new BtButton(this);
	if (page_content == NULL)
		b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	else
		page_content->addButton(b, label);
	b->setImage(iconName);

	return b;
}

void IconPage::addPage(Page *page, int id, const QString &label, const QString &iconName, int x, int y)
{
	BtButton *b = addButton(id, label, iconName, x, y);

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


IconContent::IconContent(QWidget *parent) : QWidget(parent)
{
	current_page = 0;
	QGridLayout *l = new QGridLayout(this);
	// TODO some internal page have 25px margins on both sides
	l->setContentsMargins(25, 0, 100, 0);
	l->setSpacing(28);
	l->setColumnStretch(5, 1);
	need_update = true;
}

void IconContent::addButton(QWidget *button, const QString &label)
{
	QWidget *w = button;

	if (!label.isNull())
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

void IconContent::resetIndex()
{
	need_update = true;
	current_page = 0;
}

void IconContent::showEvent(QShowEvent *e)
{
	drawContent();
	QWidget::showEvent(e);
}

void IconContent::drawContent()
{
	if (!need_update)
		return;

	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		int total_height[4];
		int area_height = contentsRect().height();

		for (int k = 0; k < 4; ++k)
			total_height[k] = 0;
		pages.append(0);

		for (int i = 0; i < items.size(); i += 4)
		{
			for (int j = 0; j < 4 && i + j < items.size(); ++j)
			{
				l->addWidget(items.at(i + j), i / 4, j);
				total_height[j] += items.at(i + j)->sizeHint().height() + l->spacing();
			}
			for (int j = 0; j < 4; ++j)
			{
				if (total_height[j] > area_height)
				{
					for (int k = 0; k < 4; ++k)
						total_height[k] = 0;
					pages.append(i);
					i -= 4;
					break;
				}
			}
		}

		pages.append(items.size());
		l->setRowStretch(l->rowCount(), 1);
	}

	emit displayScrollButtons(pageCount() > 1);
	emit contentScrolled(current_page, pageCount());

	need_update = false;

	for (int i = 0; i < items.size(); ++i)
		items[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
}

void IconContent::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	need_update = true;
	drawContent();
}

void IconContent::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	need_update = true;
	drawContent();
}

int IconContent::pageCount() const
{
	return pages.size() - 1;
}

