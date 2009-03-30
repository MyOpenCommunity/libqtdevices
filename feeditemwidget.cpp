/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A class to display a feed
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "feeditemwidget.h"
#include "fontmanager.h"
#include "main.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QRegExp>
#include <QDebug>

FeedItemWidget::FeedItemWidget(QWidget *parent) : QWidget(parent)
{
	main_layout = new QVBoxLayout(this);
	text_area = new QTextEdit;
	text_area->setReadOnly(true);
	text_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	text_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	text_area->setFrameShape(QFrame::NoFrame);
	main_layout->addWidget(text_area);
}

void FeedItemWidget::removeImages(QString &html)
{
	QRegExp image_alternate("<img[^>]*alt=\"(\\w*)\"[^>]*/>", Qt::CaseInsensitive);
	html.contains(image_alternate);
	if (!image_alternate.cap(1).isEmpty())
	{
		qDebug() << "image alternate present";
		// replace img tag with alternate text
		html.replace(image_alternate, "[" + image_alternate.cap(1) + "]");
	}
	else
	{
		qDebug() << "image alternate not present";
		// remove image
		QRegExp img_remove("<img[^>]+/>", Qt::CaseInsensitive);
		html.remove(img_remove);
	}
}

void FeedItemWidget::setFeedInfo(const FeedItemInfo &feed_item)
{
	text_area->insertHtml("<h2>" + feed_item.title + "</h2><br>");
	text_area->insertHtml("<h2>" + feed_item.last_updated + "</h2><br>");
	QString descr = feed_item.description;
	removeImages(descr);
	text_area->insertHtml(descr);
	text_area->moveCursor(QTextCursor::Start);
}

void FeedItemWidget::scrollUp()
{
	QScrollBar *vbar = text_area->verticalScrollBar();
	vbar->setValue(vbar->value() - vbar->pageStep());
}

void FeedItemWidget::scrollDown()
{
	QScrollBar *vbar = text_area->verticalScrollBar();
	vbar->setValue(vbar->value() + vbar->pageStep());
}

void FeedItemWidget::hideEvent(QHideEvent *e)
{
	text_area->clear();
}
