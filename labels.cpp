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


#include "labels.h"
#include "skinmanager.h"
#include "fontmanager.h" //bt_global::font
#include "icondispatcher.h" //bt_global::icons_cache

#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QTextDocument>
#include <QDebug>


#define SCROLL_INTERVAL 500


ScrollingLabel::ScrollingLabel(QWidget *parent)
	: QLabel(parent)
{
	init();
}

ScrollingLabel::ScrollingLabel(const QString &text, QWidget *parent)
	: QLabel(parent)
{
	init();
	setScrollingText(text);
}

ScrollingLabel::~ScrollingLabel()
{
	if (timer_id != 0)
		killTimer(timer_id);
}

void ScrollingLabel::setScrollingText(const QString &text)
{
	setText(text);
	QTimer::singleShot(0, this, SLOT(checkWidth()));
}

void ScrollingLabel::paintEvent(QPaintEvent *e)
{
	if (scrolling_needed)
	{
		QPainter p(this);
		QString full_text = text() + separator;
		int full_width = fontMetrics().width(full_text);

		int x = -offset;
		while (x < width())
		{
			p.drawText(x, 0, full_width, height(), Qt::AlignLeft | Qt::AlignVCenter, full_text);
			x += full_width;
		}
	}
	else
		QLabel::paintEvent(e);
}

void ScrollingLabel::timerEvent(QTimerEvent *e)
{
	if (e->timerId() == timer_id)
	{
		offset += fontMetrics().averageCharWidth();
		if (offset >= fontMetrics().width(text()))
			offset = 0;
		scroll(-fontMetrics().averageCharWidth(), 0);
	}
	else
		QWidget::timerEvent(e);
}

void ScrollingLabel::showEvent(QShowEvent *e)
{
	if (scrolling_needed)
		timer_id = startTimer(SCROLL_INTERVAL);
	QLabel::showEvent(e);
}

void ScrollingLabel::hideEvent(QHideEvent *e)
{
	if (timer_id != 0)
	{
		killTimer(timer_id);
		timer_id = 0;
		offset = 0;
	}
	QLabel::hideEvent(e);
}

void ScrollingLabel::resizeEvent(QResizeEvent *e)
{
	QLabel::resizeEvent(e);
	checkWidth();
}

void ScrollingLabel::init()
{
	scrolling_needed = false;
	offset = 0;
	timer_id = 0;
	separator = " -- ";
}

void ScrollingLabel::checkWidth()
{
	if (width() < fontMetrics().width(text()))
	{
		if (timer_id != 0)
			killTimer(timer_id);
		timer_id = startTimer(SCROLL_INTERVAL);
		scrolling_needed = true;
	}
	else
	{
		killTimer(timer_id);
		timer_id = 0;
		scrolling_needed = false;
	}
}


TextOnImageLabel::TextOnImageLabel(QWidget *parent, const QString &text) : QLabel(parent)
{
	setInternalText(text);
	setAlignment(Qt::AlignHCenter);
}

void TextOnImageLabel::setInternalText(const QString &text)
{
	internal_text = text;
	update();
}

void TextOnImageLabel::setBackgroundImage(const QString &path)
{
	setPixmap(*bt_global::icons_cache.getIcon(path));
}

void TextOnImageLabel::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QTextDocument td;
	td.setDefaultStyleSheet(QString("* {color: %1}").arg(_text_color));
	td.setDefaultFont(font());
	td.setHtml("<p>" + internal_text + "</p>");
	// find the correct coordinates to center the text
	QFontMetrics fm(td.defaultFont());
	QRect dim = rect();
	QPainter p(this);
	// +3 at the end is empyrical, centers correctly the text on the label
	int y = dim.height() / 2 - fm.ascent() + 3;
	int x = (dim.width() - fm.width(internal_text)) / 2;
	p.translate(x, y);
	td.drawContents(&p, QRectF(rect()));
}

QString TextOnImageLabel::textColor()
{
	return _text_color;
}

void TextOnImageLabel::setTextColor(QString color)
{
	_text_color = color;
}

