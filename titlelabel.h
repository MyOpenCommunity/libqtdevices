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


#ifndef TITLE_LABEL_H
#define TITLE_LABEL_H

#include <QString>
#include <QLabel>
#include <QTimer>

/**
 * \class TitleLabel
 *
 * this class is derived from QLabel
 * and reimplements drawContent to have scrolling text
 */
class TitleLabel : public QLabel
{
Q_OBJECT
public:
	TitleLabel(QWidget *parent = 0, int w = 0, int h = 0, int w_offset = 0, int h_offset = 0, bool scrolling = false);

	// TODO: setText era un metodo virtual di QLabel, adesso non lo e' piu'..
	// riscrivere il funzionamento di questa classe tenendo conto di questa cosa!
	void setText(const QString & text_to_set);
	void resetTextPosition();
	void setMaxVisibleChars(int n);

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	// internal data
	QString text;

	// Total chars
	unsigned text_length;

	int     w_offset;
	int     h_offset;
	bool    scrolling;

	// Text scrolling Timer
	QTimer scrolling_timer;

	/*
	 * Text scrolling parameters.
	 */
	int time_per_step;
	// current chars shifted
	unsigned current_shift;
	// how many chars can be shown at the same time_shift
	unsigned visible_chars;

	QString separator;

	/// refresh text
	void refreshText();

public slots:
	void handleScrollingTimer();
};


class TextOnImageLabel : public QLabel
{
Q_OBJECT
Q_PROPERTY(QString text_color READ textColor WRITE setTextColor)

public:
	TextOnImageLabel(QWidget *parent=0, const QString &text = QString());
	void setBackgroundImage(const QString &path);
	void setInternalText(const QString &text);

protected:
	virtual void paintEvent(QPaintEvent *);

private:
	QString internal_text;
	QString _text_color; // the internal storage for the property text_color

	QString textColor();
	void setTextColor(QString color);
};

#endif
