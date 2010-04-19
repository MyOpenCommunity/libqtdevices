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

#ifndef BANNERCONTENT_H
#define BANNERCONTENT_H

#include <QWidget>
#include <QList>

#include "page.h"
#include "gridcontent.h"

class banner;


/**
 * The BannerContent class manage a list of banner usually put inside a Page.
 */
class BannerContent : public GridContent
{
friend void BannerPage::activateLayout();
Q_OBJECT
public:
#ifdef LAYOUT_BTOUCH
	BannerContent(QWidget *parent=0);
#else
	BannerContent(QWidget *parent=0, int columns=2);
#endif

	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);
	void initBanners();

protected:
	void drawContent();

private:
	QList<banner*> banner_list;
	int columns;
};

#endif // BANNERCONTENT_H
