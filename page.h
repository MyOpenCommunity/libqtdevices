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

#include "main.h"

#include <QWidget>


class Page : public QWidget
{
Q_OBJECT
public:
	// Normally, the page is a fullscreen page, but sometimes is a part of
	// another page (see Antintrusion or SoundDiffusion)
	Page(QWidget *parent=0) : QWidget(parent) {}
	virtual void inizializza() {}

public slots:
	virtual void showPage() { showFullScreen(); setFixedSize(MAX_WIDTH, MAX_HEIGHT);}

signals:
	/// Emitted when the page is closed.
	void Closed();
};

#endif
