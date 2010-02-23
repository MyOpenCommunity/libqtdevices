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


#ifndef CONTENT_WIDGET_H
#define CONTENT_WIDGET_H

#include <QWidget>
#include <QList>

#include "page.h"

class banner;
class QShowEvent;

class BannerContentBase : public QWidget
{
friend void BannerPage::activateLayout();
Q_OBJECT
public:
	BannerContentBase(QWidget *parent=0);
	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);
	void initBanners();

public slots:
	virtual void pgUp() = 0;
	virtual void pgDown() = 0;
	virtual void resetIndex() = 0;

protected:
	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	virtual void drawContent() = 0;
	QList<banner*> banner_list;

signals:
	void displayScrollButtons(bool display);
	void contentScrolled(int current, int total);
};


class BannerContent : public BannerContentBase
{
Q_OBJECT
public:
	BannerContent();

public slots:
	virtual void pgUp();
	virtual void pgDown();
	virtual void resetIndex();

protected:
	virtual void drawContent();

private:
	int pageCount() const;
	void prepareLayout(QList<QWidget *> items, int columns);
	void updateLayout(QList<QWidget *> items);

	bool need_update;
	int current_page;
	QList<int> pages;
};

#endif // CONTENT_WIDGET_H
