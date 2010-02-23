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


#ifndef LIST_BROWSER_H
#define LIST_BROWSER_H

#include <QVector>
#include <QString>
#include <QWidget>

class QButtonGroup;
class QVBoxLayout;
class QBoxLayout;
class QLabel;


/**
 * \class ListBrowser
 *
 * This class show a list of item and supply a list of signal/slot for handling
 * the user choices.
 */
class ListBrowser : public QWidget
{
Q_OBJECT
public:
	ListBrowser(QWidget *parent, unsigned rows_per_page);

	void setList(QVector<QString> _item_list, unsigned _current_page = 0);
	void showList();

	unsigned getCurrentPage();

signals:
	void itemIsClicked(int item);

public slots:
	void nextItem();
	void prevItem();

private:
	/// For pagination
	unsigned rows_per_page;
	unsigned current_page;

	QButtonGroup *buttons_group;
	QVBoxLayout *main_layout;

	/// The list of items displayed
	QVector<QString> item_list;

	void addHorizontalBox(QBoxLayout *layout, QLabel *label, int id_btn);

private slots:
	void clicked(int item);
};

#endif
