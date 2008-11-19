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


#include "titlelabel.h"

#include <QVector>
#include <QString>
#include <QWidget>


class ButtonsBar;

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
	ListBrowser(QWidget *parent, unsigned rows_per_page, Qt::WindowFlags f=0);
	~ListBrowser();

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

	QVector<TitleLabel*> labels_list;
	ButtonsBar             *buttons_bar;

	/// The list of items displayed
	QVector<QString> item_list;

	/// before to show itself some init is done
	void showEvent(QShowEvent *event);

private slots:
	void clicked(int item);
};

#endif
