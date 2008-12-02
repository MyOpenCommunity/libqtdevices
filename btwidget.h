/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This is the main class of all widget used in BTouch app.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef BTWIDGET_H
#define BTWIDGET_H

#include <QWidget>

class BTWidget : public QWidget
{
Q_OBJECT
public:
	BTWidget(QWidget *parent=0);
signals:
	/// Emitted when the widget is closed.
	void Closed();
};

#endif
