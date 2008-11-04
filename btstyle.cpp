#include "btstyle.h"

void BtStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal,
	bool enabled,const QString &text, QPalette::ColorRole textRole) const
{
	QStyle::drawItemText(painter, rect, flags, pal, true, text, textRole);
}

QPixmap BtStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,const QStyleOption *opt) const
{
	return QCommonStyle::generatedIconPixmap(QIcon::Normal, pixmap, opt);
}
