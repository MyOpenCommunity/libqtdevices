/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements a label scrollable.
 *
 *
 * \author Alessandro Della Villa <kemosh@develer.com>
 * \date December 2007
 */

#ifndef TITLE_LABEL_H
#define TITLE_LABEL_H

#include "btlabel.h"

#include <QString>
#include <QTimer>

/**
 * \class TitleLabel
 *
 * this class is derived from BtLabel
 * and reimplements drawContent to have scrolling text
 */
class TitleLabel : public BtLabel
{
Q_OBJECT
public:
	TitleLabel(QWidget *parent = 0, int w = 0, int h = 0, int w_offset = 0, int h_offset = 0, bool scrolling = FALSE, Qt::WindowFlags f = 0);

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

#endif
