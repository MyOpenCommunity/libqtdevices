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

#include <qvaluevector.h>
#include <qstring.h>
#include <qwidget.h>
#include <qptrvector.h>

#include "titlelabel.h"

class ButtonsBar;

class ListBrowser : public QWidget
{
Q_OBJECT
public:
	ListBrowser(QWidget *parent, unsigned rows_per_page, const char *name=0, WFlags f=0);

	/// Apply Style
	void setBGColor(QColor c);
	void setFGColor(QColor c);

	void setList(QValueVector<QString> _item_list, unsigned _current_page = 0);
	void showList();

	unsigned getCurrentPage();

signals:
	void itemIsClicked(QString item);

public slots:
	void nextItem();
	void prevItem();

private:
	/// For pagination
	unsigned rows_per_page;
	unsigned current_page;

	QPtrVector<TitleLabel> labels_list;
	ButtonsBar             *buttons_bar;

	/// The list of items displayed
	QValueVector<QString> item_list;

	/// before to show itself some init is done
	void showEvent(QShowEvent *event);

private slots:
	void itemIsClicked(int item);
};

#endif
