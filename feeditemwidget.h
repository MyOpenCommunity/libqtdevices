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


#ifndef FEEDITEMWIDGET_H
#define FEEDITEMWIDGET_H

#include "feedparser.h"

#include <QWidget>

class QVBoxLayout;
class QTextEdit;

class FeedItemWidget : public QWidget
{
Q_OBJECT
public:
	FeedItemWidget(QWidget *parent = 0);
	void setFeedInfo(const FeedItemInfo &feed_item);

public slots:
	void scrollUp();
	void scrollDown();

protected:
	void hideEvent(QHideEvent *e);

private:
	/**
	 * Remove image tags from html. QTextBrowser doesn't load automatically images from the net
	 * so we should reimplement this behaviour. Furthermore, if images were too big, we would need
	 * horizonatal scroll bars.
	 * \param html A text in html format
	 */
	void removeImages(QString &html);

	// remove <a> tags
	void removeLinks(QString &html);

	QVBoxLayout *main_layout;
	QTextEdit *text_area;
};

#endif // FEEDITEMWIDGET_H
