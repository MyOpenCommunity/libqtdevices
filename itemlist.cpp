#include "itemlist.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache

#include <QButtonGroup>
#include <QLayout>
#include <QDebug>
#include <QLabel>
#include <QFont>


ItemList::ItemInfo::ItemInfo(QString n, QString descr, QString i, QString bi, QVariant d)
{
	name = n;
	description = descr;
	icon = i;
	button_icon = bi;
	data = d;
}


ItemList::ItemList(QWidget *parent, int _rows_per_page)
	: QWidget(parent)
{
	// Set the number of elements shown
	rows_per_page = _rows_per_page;
	current_page = 0;

	main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 5, 0);
	main_layout->setSpacing(3);
	buttons_group = new QButtonGroup(this);
	connect(buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
}

void ItemList::addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn)
{
	QFont font = bt_global::font->get(FontManager::TEXT);

	// top level widget (to set background using stylesheet)
	QWidget *boxWidget = new QWidget;
	boxWidget->setFixedHeight(68);

	QHBoxLayout *box = new QHBoxLayout(boxWidget);
	box->setContentsMargins(5, 5, 5, 5);

	// file icon on the left
	QLabel *icon = new QLabel;
	icon->setPixmap(*bt_global::icons_cache.getIcon(item.icon));
	box->addWidget(icon);

	// centered file name and description
	QVBoxLayout *labels = new QVBoxLayout;
	QLabel *name = new QLabel(item.name);
	QLabel *description = new QLabel(item.description);
	name->setFont(font);
	description->setFont(font);

	labels->addWidget(name, 1);
	labels->addWidget(description, 1);

	box->addLayout(labels, 1);

	// button on the right
	BtButton *btn = new BtButton;
	btn->setImage(item.button_icon);
	box->addWidget(btn, 0, Qt::AlignRight);

	buttons_group->addButton(btn, id_btn);
	layout->addWidget(boxWidget);
}

void ItemList::showList()
{
	int start = current_page * rows_per_page;
	int count = qMin(start + rows_per_page, item_list.count()) - start;

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
	main_layout->addStretch();
}

void ItemList::setList(QList<ItemInfo> items, int page)
{
	item_list = items;
	current_page = page;
}

unsigned ItemList::getCurrentPage()
{
	return current_page;
}

void ItemList::nextItem()
{
	current_page += 1;
	// wrap around to the first page
	if (current_page >= pageCount())
		current_page = 0;
	showList();
}

void ItemList::prevItem()
{
	current_page -= 1;
	// wrap around to the last page
	if (current_page < 0)
		current_page = pageCount() - 1;
	showList();
}

void ItemList::clicked(int item)
{
	emit itemIsClicked(current_page * rows_per_page + item);
}

int ItemList::pageCount()
{
	return item_list.count() / rows_per_page +
			(item_list.count() % rows_per_page ? 1 : 0);
}
