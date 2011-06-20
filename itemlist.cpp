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


#include "itemlist.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "labels.h" // ScrollingLabel

#include <QButtonGroup>
#include <QLayout>
#include <QDebug>
#include <QLabel>
#include <QFont>

// The default index of each icon type
#ifdef LAYOUT_TS_10
enum
{
	ITEM_ICON = 0,
	BUTTON_ICON
};
#else
enum
{
	BUTTON_ICON = 0
};

#endif


ItemList::ItemInfo::ItemInfo(QString n, QString descr, QStringList i, QVariant d)
{
	name = n;
	description = descr;
	icons = i;
	data = d;
}


ItemList::ItemList(int _rows_per_page, QWidget *parent) :
	ScrollableContent(parent)
{
	// Set the number of elements shown
	rows_per_page = _rows_per_page;

	QGridLayout *main_layout = static_cast<QGridLayout *>(layout());

#ifdef LAYOUT_TS_10
	main_layout->setContentsMargins(0, 5, 5, 0);
	main_layout->setSpacing(3);
#else
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(10);
#endif
	buttons_group = new QButtonGroup(this);
	connect(buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
}

void ItemList::addHorizontalBox(QGridLayout *layout, const ItemInfo &item, int id_btn)
{
	QFont font = bt_global::font->get(FontManager::TEXT);

	// top level widget (to set background on ts 10 using stylesheet)
	QWidget *box_widget = new QWidget;
	QHBoxLayout *box = new QHBoxLayout(box_widget);

#ifdef LAYOUT_TS_10
	Q_ASSERT_X(item.icons.size() >= 2, "ItemList::addHorizontalBox()", "too few icons to construct the horizontal box");

	box_widget->setFixedHeight(68);

	box->setContentsMargins(5, 5, 5, 5);

	// file icon on the left
	QLabel *icon = new QLabel;
	icon->setPixmap(*bt_global::icons_cache.getIcon(item.icons[ITEM_ICON]));
	box->addWidget(icon);

	// centered file name and description
	QVBoxLayout *labels = new QVBoxLayout;
	QLabel *name = new ScrollingLabel(item.name);
	QLabel *description = new ScrollingLabel(item.description);
	name->setFont(font);
	description->setFont(font);

	labels->addWidget(name, 1);
	labels->addWidget(description, 1);

	box->addLayout(labels, 1);
#else
	Q_ASSERT_X(item.icons.size() == 1, "ItemList::addHorizontalBox()", "wrong number of icons to build the horizontal box");
	box->setContentsMargins(0, 0, 0, 0);

	Q_UNUSED(item.description)
	QLabel *name = new ScrollingLabel(item.name);
	name->setFont(font);

	box->addWidget(name);
#endif

	// button on the right
	BtButton *btn = new BtButton(item.icons[BUTTON_ICON]);
	box->addWidget(btn, 0, Qt::AlignRight);

	buttons_group->addButton(btn, id_btn);

	layout->addWidget(box_widget, layout->rowCount(), 0);
}

void ItemList::drawContent()
{
	showList();
}

void ItemList::showList()
{
	need_update = false;

	int start = current_page * rows_per_page;
	int count = qMin(start + rows_per_page, item_list.count()) - start;

	QGridLayout *main_layout = static_cast<QGridLayout *>(layout());

	// Remove the old children of main_layout
	while (QLayoutItem *child = main_layout->takeAt(0))
	{
		if (QWidget *w = child->widget())
		{
			w->disconnect();
			w->hide();
			w->deleteLater();
		}
	}

	emit displayScrollButtons(item_list.count() > rows_per_page);
	emit contentScrolled(current_page, pageCount());

	for (int i = 0; i < count; ++i)
		addHorizontalBox(main_layout, item_list[start + i], i);
	main_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding), main_layout->rowCount(), 0);
}

void ItemList::setList(QList<ItemInfo> items, int page)
{
	item_list = items;
	current_page = page;
	need_update = true;
}

ItemList::ItemInfo &ItemList::item(int index)
{
	Q_ASSERT_X(index >= 0 && index < item_list.size(), "ItemList::item", "index out of range");
	return item_list[index];
}

int ItemList::itemCount()
{
	return item_list.size();
}

void ItemList::insertItem(int index, const ItemInfo &item)
{
	Q_ASSERT_X(index >= 0 && index <= item_list.size(), "ItemList::insertItem", "index out of range");

	item_list.insert(index, item);
	need_update = true;
}

void ItemList::removeItem(int index)
{
	Q_ASSERT_X(index >= 0 && index < item_list.size(), "ItemList::item", "index out of range");
	item_list.removeAt(index);
	int start = current_page * rows_per_page;
	if (start >= item_list.size())
		--current_page;

	if (current_page < 0)
		current_page = 0;
	need_update = true;
}

unsigned ItemList::getCurrentPage()
{
	return current_page;
}

void ItemList::clear()
{
	setList(QList<ItemInfo>());
	showList();
}

void ItemList::clicked(int item)
{
	emit itemIsClicked(current_page * rows_per_page + item);
}

int ItemList::pageCount() const
{
	return item_list.count() / rows_per_page +
			(item_list.count() % rows_per_page ? 1 : 0);
}
