/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class represent a Page.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef PAGE_H
#define PAGE_H

#include <QWidget>


class Page : public QWidget
{
Q_OBJECT
public slots:
	virtual void showPage() { showFullScreen(); }

signals:
	/// Emitted when the page is closed.
	void Closed();
};

#endif
