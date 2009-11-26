#include "styledwidget.h"

#include <QStyleOption>
#include <QPainter>


StyledWidget::StyledWidget(QWidget *parent)
	: QWidget(parent)
{
}

void StyledWidget::paintEvent(QPaintEvent *)
{
	// required for Style Sheets on a QWidget subclass
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
