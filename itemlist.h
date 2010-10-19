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


#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QVariant>


class QButtonGroup;
class QVBoxLayout;
class QBoxLayout;
class QLabel;

/*!
	\ingroup Core
	\brief Class to show a list of items

	This class show a list of items (like files or other "virtual" items) and
	supply a list of signal/slot for handling the user choices.

	It works like the ScrollableContent class, but it can display only homogeneus
	items.

	The structure to represent a single item is the ItemList::ItemInfo structure.
	You can set a list of this structure to the ItemList using the setList()
	method. You can insert a single item, too, using the insertItem() method.
	Items can be removed one a time using the removeItem() method or all together
	using the clear() method.
	To retrieve or access directly an item, you can use the item() method.
	The total number of items contained into the ItemList can be taken using the
	itemCount() method.

	To customize the look&feel of the ItemInfo rappresentation on the ItemList,
	you can reimplement the addHorizontalBanner() method.
*/
class ItemList : public QWidget
{
Q_OBJECT
public:
	/*!
		\brief Structure to represent an item.

		An item can have a name, a description a list of icons.
		It has a QVariant field to store any kind of data the user wants.
	*/
	struct ItemInfo
	{
		ItemInfo(QString name = QString(), QString desc = QString(),
			 QStringList icons = QStringList(), QVariant data = QVariant());

		QString name;
		QString description;
		QStringList icons;
		QVariant data;
	};

	/*!
		\brief Constructor.

		Construct a new ItemList with the given \a parent and arranging the
		items in \a rows_per_page rows.
	*/
	ItemList(QWidget *parent, int rows_per_page);

	/*!
		\brief Sets the list of items.

		Sets \a items to be the content of the item list, and switch to the page
		\a page (the number of pages is calculated using the
		ItemList::rows_per_page member).
	*/
	void setList(QList<ItemInfo> items, int page = 0);

	/*!
		\brief Shows the subset of items of the current page.
	*/
	void showList();

	/*!
		\brief Returns the reference to the ItemInfo structure of the
		item \a index.
	*/
	ItemInfo &item(int index);

	/*!
		\brief Returns the total count of items contained into the ItemList.
	*/
	int itemCount();

	/*!
		\brief Inserts \a item at position \a index.

		The successive items are moved forward.
		If \a index is 0, the \a item is prepended to the list.
		If \a index is \a itemCount(), the value is appended to the list.
	 */
	void insertItem(int index, const ItemInfo &item);

	/*!
		\brief Removes the item at position \a index from the ItemList.
	*/
	void removeItem(int index);

	/*!
		\brief Returns the current page index.
	*/
	unsigned getCurrentPage();

signals:
	/*!
		\brief Emitted when an item is clicked.
	*/
	void itemIsClicked(int item);

	/*!
		\brief Emitted when the layout is updated.

		\a display is true if scrolling is needed, false otherwise.
	*/
	void displayScrollButtons(bool);

	/*!
		\brief Emitted when the content is scrolled.

		\a current is the index of the current displayed page,
		\a total is the total number of pages.
	*/
	void contentScrolled(int current, int total);

public slots:
	/*!
		\brief Switches to the next page.

		\note If you are on the last page, it switches to the first one.
	*/
	void nextItem();

	/*!
		\brief Switches to the previous page.

		\note If you are on the first page, it switches to the last one.
	*/
	void prevItem();

	/*!
		\brief Removes all the items.
	*/
	void clear();

protected slots:
	/*!
		\brief Emits the itemIsClicked() signal.

		\note \a item must be the index of the item into the current page.
		For example, if ItemList::rows_per_page is 4, and you have 16 elements,
		to make the itemIsClicked() signal to be emitted with 11 as argument,
		you must be on the 3rd page and call this method with 4 as parameter.
	*/
	void clicked(int item);

protected:
	/*!
		\brief The maximum number of rows a page can have.
	*/
	int rows_per_page;

	/*!
		\brief Current page index.
	*/
	int current_page;

	QButtonGroup *buttons_group;
	QVBoxLayout *main_layout;

	/*!
		\brief The list of items displayed.
	*/
	QList<ItemInfo> item_list;

	/*!
		\brief The calculated number of pages.
	*/
	int pageCount();

	/*!
		\brief Creates the widget used to represent an item.

		In the default implementation, creates a box with a left icon, a label
		with the \a item name, a label with the \a item description and a button.

		\note \a item.icons must contain at least 2 entries: one for the left
		icon and one for the button.
	*/
	virtual void addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn);
};

#endif
