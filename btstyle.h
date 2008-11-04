/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements a custom style for widgets
 *
 * This class inherits QCommonStyle to prevent the "graying" of the text/image.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#ifndef BTSTYLE_H
#define BTSTYLE_H

#include <QCommonStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QString>
#include <QIcon>


class BtStyle : public QCommonStyle
{
Q_OBJECT
public:
	virtual void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal,
		bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;

	virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
		const QStyleOption *opt) const;
};

#endif
