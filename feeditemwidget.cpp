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


#include "feeditemwidget.h"
#include "fontmanager.h"
#include "main.h"
#include "btbutton.h"
#include "skinmanager.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QRegExp>
#include <QDebug>

FeedItemWidget::FeedItemWidget(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	text_area = new QTextEdit;
	text_area->setTextInteractionFlags(Qt::NoTextInteraction);
	text_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	text_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	text_area->setFrameShape(QFrame::NoFrame);
	main_layout->addWidget(text_area);

	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons_layout->addStretch();

	BtButton *prev_button = new BtButton;
	prev_button->setImage(bt_global::skin->getImage("previous"));
	connect(prev_button, SIGNAL(clicked()), SIGNAL(showPrev()));
	buttons_layout->addWidget(prev_button);

	BtButton *next_button = new BtButton;
	next_button->setImage(bt_global::skin->getImage("next"));
	connect(next_button, SIGNAL(clicked()), SIGNAL(showNext()));
	buttons_layout->addWidget(next_button);

	main_layout->addLayout(buttons_layout);
}

void FeedItemWidget::removeImages(QString &html)
{
	QRegExp image_alternate("<img[^>]*(?:alt=\"([^\"]*)\")?[^>]*>", Qt::CaseInsensitive);
	// capture only the smallest subset
	image_alternate.setMinimal(true);
	int index = html.indexOf(image_alternate);
	while (index >= 0)
	{
		QString alt;
		if (!image_alternate.cap(1).isEmpty())
		{
			qDebug() << "image alternate present";
			// replace img tag with alternate text
			alt = "[" + image_alternate.cap(1) + "]";
		}
		else
		{
			qDebug() << "image alternate not present";
			// remove image
		}
		html.replace(index, image_alternate.matchedLength(), alt);
		index = html.indexOf(image_alternate);
	}
}

void FeedItemWidget::removeLinks(QString &html)
{
	QRegExp a_tags("<a[^>]*>");

	html.remove(a_tags);
}

void FeedItemWidget::setFeedInfo(const FeedItemInfo &feed_item)
{
	text_area->insertHtml("<h2>" + feed_item.title + "</h2><br>");
	text_area->insertHtml("<h2>" + feed_item.last_updated + "</h2><br>");
	QString descr = feed_item.description;
	removeImages(descr);
	removeLinks(descr);
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
