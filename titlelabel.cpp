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


#include "titlelabel.h"
#include "skinmanager.h"
#include "fontmanager.h" //bt_global::font
#include "icondispatcher.h" //bt_global::icons_cache

#include <QTimer>
#include <QPainter>
#include <QTextDocument>
#include <QDebug>


ScrollingLabel::ScrollingLabel(const QString &text, QWidget *parent) : QLabel(parent)
{
	separator = "   --   ";

	timer = new QTimer(this);
	timer->setInterval(333);
	connect(timer, SIGNAL(timeout()), SLOT(handleScroll()));

	setScrollingText(text);
}

ScrollingLabel::ScrollingLabel(QWidget *parent) : QLabel(parent)
{
	separator = "   --   ";

	timer = new QTimer(this);
	timer->setInterval(333);
	connect(timer, SIGNAL(timeout()), SLOT(handleScroll()));
}

void ScrollingLabel::setScrollingText(const QString &text)
{
	scrolling_text = text;
	setText(scrolling_text);
	// We use a single shot to allow a scenario like this (because the
	// checkScrolling use the fontMetrics):
	// label->setScrollingText()
	// label->setFont()
	QTimer::singleShot(0, this, SLOT(checkScrolling()));
}

void ScrollingLabel::resizeEvent(QResizeEvent *e)
{
	QLabel::resizeEvent(e);
	int displayable_chars = width() / fontMetrics().averageCharWidth();
	setText(scrolling_text.left(displayable_chars));
	checkScrolling();
}

void ScrollingLabel::hideEvent(QHideEvent *e)
{
	QLabel::hideEvent(e);
	timer->stop();
}

void ScrollingLabel::showEvent(QShowEvent *e)
{
	QLabel::showEvent(e);
	int displayable_chars = width() / fontMetrics().averageCharWidth();
	setText(scrolling_text.left(displayable_chars));
	checkScrolling();
}

void ScrollingLabel::checkScrolling()
{
	text_offset = 0;
	timer->stop();
	if (fontMetrics().width(scrolling_text) > width())
		QTimer::singleShot(200, timer, SLOT(start())); // empirical delay
}

void ScrollingLabel::handleScroll()
{
	QString entire_text = scrolling_text + separator;

	int displayable_chars = width() / fontMetrics().averageCharWidth();

	if (text_offset < entire_text.length())
		++text_offset;
	else
		text_offset = 0;

	QString to_display = entire_text.mid(text_offset) + entire_text.left(text_offset);
	setText(to_display.left(displayable_chars));
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

