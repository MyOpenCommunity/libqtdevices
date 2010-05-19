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


#ifndef LABELS_H
#define LABELS_H

#include <QString>
#include <QLabel>

class QResizeEvent;
class QShowEvent;
class QHideEvent;
class QTimerEvent;
class QPaintEvent;

/**
 * A label that scrolls its text one char at time from left to right.
 */
class ScrollingLabel : public QLabel
{
	Q_OBJECT

public:
	ScrollingLabel(QWidget *parent = 0);
	ScrollingLabel(const QString &text, QWidget *parent = 0);
	~ScrollingLabel();

	void setScrollingText(const QString &text);

protected:
using QLabel::setText;
	void paintEvent(QPaintEvent *e);
	void timerEvent(QTimerEvent *e);
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);
	void resizeEvent(QResizeEvent *e);

private slots:
	void checkWidth();

private:
	void init();

	bool scrolling_needed;
	int offset;
	int timer_id;
	QString separator;
};


/**
 * A label that can have both the text and a background image.
 */
class TextOnImageLabel : public QLabel
{
Q_OBJECT
Q_PROPERTY(QString text_color READ textColor WRITE setTextColor)

public:
	TextOnImageLabel(QWidget *parent=0, const QString &text = QString());
	void setBackgroundImage(const QString &path);
	void setInternalText(const QString &text);

protected:
using QLabel::setText;
	virtual void paintEvent(QPaintEvent *);

private:
	QString internal_text;
	QString _text_color; // the internal storage for the property text_color

	QString textColor();
	void setTextColor(QString color);
};

#endif
