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

#ifndef BTOUCH_PDFPAGE_H
#define BTOUCH_PDFPAGE_H

#include "page.h"
#include <poppler/qt4/poppler-qt4.h>

class QLabel;


class PdfPage : public Page
{
Q_OBJECT
public:
	PdfPage();
	virtual ~PdfPage();

	// load the PDF and display the first page
	void displayPdf(const QString &path);

	// display 0-based page of the current PDF file
	void displayPage(int index);

	virtual void showPage();

private slots:
	void scrollDown();
	void scrollUp();

private:
	// display the visible page part starting from y_offset
	void displayPagePart();

private:
	Poppler::Document *pdf_document;
	QLabel *pdf_page;
	QImage pdf_image;
	int current_page, y_offset;
};

#endif
