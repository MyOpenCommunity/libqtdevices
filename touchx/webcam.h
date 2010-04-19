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


#ifndef WEBCAM_LIST_H
#define WEBCAM_LIST_H

#include "page.h"
#include "itemlist.h"

class QDomNode;
class QHttp;
class QUrl;
class QString;
class ImageLabel;


/**
 * The page that display a webcam image, plus some controls to reload/change image
 */
class WebcamPage : public Page
{
Q_OBJECT
public:
	WebcamPage();
	void setImage(QUrl url, QString label);

signals:
	void reloadImage();
	void nextImage();
	void prevImage();

private slots:
	void downloadFinished(int id, bool error);

private:
	PageTitleWidget *page_title;
	ImageLabel *image;
	QHttp *http;
	QString description;
};


/**
 * This page shows the list of webcam, taking them from the configuration file.
 */
class WebcamListPage : public Page
{
Q_OBJECT
public:
	WebcamListPage(const QDomNode &config_node);

	typedef ItemList ContentType;

private slots:
	void itemSelected(int index);
	void reloadImage();
	void nextImage();
	void prevImage();

private:
	int current_image; // the index of the current webcam image
	WebcamPage *webcam_page;

	void loadWebcams(const QDomNode &config_node);
};

#endif // WEBCAM_LIST_H
