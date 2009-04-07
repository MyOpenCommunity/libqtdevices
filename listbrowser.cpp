/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** listbrowser.cpp
 **
 ** browser of a list of items.
 **
 ****************************************************************/

#include "listbrowser.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "main.h" // MAX_WIDTH
#include "titlelabel.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName
#include "skinmanager.h" // bt_global::skin

#include <QButtonGroup>
#include <QLayout>
#include <QDebug>
#include <QLabel>
#include <QFont>

ListBrowser::ListBrowser(QWidget *parent, unsigned _rows_per_page) : QWidget(parent)
{
	// Set the number of elements shown
	rows_per_page = _rows_per_page;
	current_page = 0;

	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 0);
	main_layout->setSpacing(8);
	buttons_group = new QButtonGroup(this);
	connect(buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
}

BtButton *getTrimmedButton(QWidget *parent, const QString &icon)
{
#define BTN_WIDTH 50
#define BTN_HEIGHT 50
	BtButton *btn = new BtButton(parent);
	QPixmap tmp = (*bt_global::icons_cache.getIcon(icon)).copy(5, 5, BTN_WIDTH, BTN_HEIGHT);
	btn->setPixmap(tmp);
	tmp = (*bt_global::icons_cache.getIcon(getPressName(icon))).copy(5, 5, BTN_WIDTH, BTN_HEIGHT);
	btn->setPressedPixmap(tmp);
	return btn;
}

void ListBrowser::addHorizontalBox(QBoxLayout *layout, QLabel *label, int id_btn)
{
	QHBoxLayout *box = new QHBoxLayout();
	box->addWidget(label, 0, Qt::AlignLeft);
	BtButton *btn = getTrimmedButton(0, bt_global::skin->getImage("forward"));
	box->addWidget(btn, 0, Qt::AlignRight);
	box->setContentsMargins(5, 0, 0, 0);
	buttons_group->addButton(btn, id_btn);
	layout->addLayout(box);
}

void ListBrowser::showList()
{
	int start = current_page * rows_per_page;
	int count = qMin(static_cast<int>(start + rows_per_page), item_list.count()) - start;

	// Remove the old children of main_layout
	while (QLayoutItem *child = main_layout->takeAt(0))
		if (QLayout *l = child->layout())
			while (QLayoutItem *li = l->takeAt(0))
				if (QWidget *w = li->widget())
				{
					w->disconnect();
					w->hide();
					w->deleteLater();
				}

	QVector<int> h_offsets;
	h_offsets.append(-4);
	h_offsets.append(-2);
	h_offsets.append(-1);
	h_offsets.append(0);

	QFont aFont = bt_global::font->get(FontManager::TEXT);

	for (int i = 0; i < count; ++i)
	{
		TitleLabel *l = new TitleLabel(0, MAX_WIDTH - 60, 50, 9, h_offsets[i], true);
		l->setText(item_list[start + i]);
		l->setFont(aFont);
		addHorizontalBox(main_layout, l, i);
	}
	main_layout->addStretch();
}

void ListBrowser::setList(QVector<QString> _item_list, unsigned _current_page)
{
	item_list = _item_list;
	current_page = _current_page;
}

unsigned ListBrowser::getCurrentPage()
{
	return current_page;
}

void ListBrowser::nextItem()
{
	if (static_cast<int>((current_page + 1) * rows_per_page) < item_list.count())
	{
		++current_page;
		showList();
	}
}

void ListBrowser::prevItem()
{
	if (current_page)
	{
		--current_page;
		showList();
	}
}

void ListBrowser::clicked(int item)
{
	emit itemIsClicked(current_page * rows_per_page + item);
}
