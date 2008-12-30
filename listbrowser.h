/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the browsing of generic items
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date May 2008
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
