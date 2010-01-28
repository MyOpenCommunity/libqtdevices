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
