/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A widget to display a feed
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
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
	void scrollUp();
	void scrollDown();

private:
	QVBoxLayout *main_layout;
	QTextEdit *text_area;
};

#endif // FEEDITEMWIDGET_H
