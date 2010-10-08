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


#ifndef PAGE_H
#define PAGE_H

#include <QWidget>

#include "styledwidget.h"

class Client;
class PageContainer;
class QVBoxLayout;
class TransitionWidget;
class AbstractNavigationBar;


// helper widget containing a centered label and a label on the right
// displaying the current page
class PageTitleWidget : public QWidget
{
Q_OBJECT
public:
	PageTitleWidget(const QString &label, int height);

public slots:
	void setCurrentPage(int current, int total);
	void setTitle(QString title);
};


// This typedef is needed by slots valueReceived(DeviceValues). In order to avoid
// duplication the typedef is put here, so all pages can be use freely
typedef QHash<int, QVariant> DeviceValues;


#define page_content (content(this))


class Page : public StyledWidget
{
friend class BtMain;
Q_OBJECT
public:
	static const int TITLE_HEIGHT = 70;
	static const int SMALL_TITLE_HEIGHT = 35;
	static const int TINY_TITLE_HEIGHT = 20;

	typedef QWidget ContentType;

	Page(QWidget *parent = 0);
	~Page();
	virtual void inizializza();

	virtual int sectionId() const;

	static void setClients(Client *command, Client *request);

	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	static void setPageContainer(PageContainer *container);
	virtual void activateLayout();
	void addBottomWidget(QWidget *bottom);
	void forceClosed();
	virtual void aboutToHideEvent();

public slots:
	virtual void showPage();
	virtual void cleanUp();

signals:
	void Closed();
	void forwardClick();

protected:
	template<class P>
	typename P::ContentType* content(P*)
	{
		return (typename P::ContentType*)__content;
	}

	QWidget *__content;

	void prepareTransition();
	void startTransition();

	void buildPage(QWidget *main_widget, QWidget *content, AbstractNavigationBar *nav_bar,
		QWidget *top_widget = 0, QWidget *title_widget = 0);

	void buildPage(QWidget *content, AbstractNavigationBar *nav_bar, QWidget *top_widget = 0,
		QWidget *title_widget = 0);

	void buildPage(QWidget *content, AbstractNavigationBar *nav_bar, const QString& label,
		int label_height = TITLE_HEIGHT, QWidget *top_widget = 0);

private:
	static PageContainer *page_container;
	static Client *client_richieste;
	static Client *client_comandi;

	void setCurrentPage();
	Page *currentPage();
};

#endif
