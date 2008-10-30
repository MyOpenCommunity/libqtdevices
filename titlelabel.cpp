/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** titlelabel.cpp
 **
 ** a scrollable label
 **
 ****************************************************************/

#include "titlelabel.h"

#include <QPainter>


TitleLabel::TitleLabel(QWidget *parent, int w, int h, int _w_offset, int _h_offset, bool _scrolling, Qt::WindowFlags f) :
	BtLabel(parent)
{
	// Style
	setFixedWidth(w);
	setFixedHeight(h);
	setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// init
	time_per_step   = 333;
	visible_chars   = 18;
	text_length     = 0;
	current_shift   = 0;
	w_offset        = _w_offset;
	h_offset        = _h_offset;

	// separator for ciclic text scrolling
	separator        = "   --   ";

	// define if it is scrolling label or not
	scrolling = _scrolling;

	// connect timer to scroll text
	connect(&scrolling_timer, SIGNAL(timeout()), SLOT(handleScrollingTimer()));
}

void TitleLabel::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	painter.translate(w_offset, h_offset);
	painter.end();
	BtLabel::paintEvent(event);
}

void TitleLabel::resetTextPosition()
{
	current_shift = 0;
}

void TitleLabel::setText(const QString & text_to_set)
{
	// store full string and full lenght
	text         = text_to_set;
	text_length  = text_to_set.length();
	current_shift = 0;

	// call method of ancestor
	QLabel::setText(text_to_set);

	// start the timer if scroll is needed
	if (scrolling && text_length > visible_chars)
		scrolling_timer.start(time_per_step);
	else
		scrolling_timer.stop();
}

void TitleLabel::refreshText()
{
	QString banner_string = QString("%1%2").arg(text).arg(separator);

	QString head = banner_string.mid(current_shift, banner_string.length() - current_shift);
	QString tail = banner_string.mid(0, current_shift);

	QLabel::setText(QString("%1%2").arg(head).arg(tail));
}

void TitleLabel::setMaxVisibleChars(int n)
{
	visible_chars = n;
	current_shift = 0;
	refreshText();
}

void TitleLabel::handleScrollingTimer()
{
	if (current_shift < text_length + separator.length())
		++current_shift;
	else
		current_shift = 0;

	refreshText();
	repaint();
}
