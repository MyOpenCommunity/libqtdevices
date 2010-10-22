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

#include "pdfpage.h"
#include "navigation_bar.h"

#include <QLabel>
#include <QtDebug>

#define SCREEN_DPI 72.0


PdfPage::PdfPage()
{
	pdf_document = NULL;
	current_page = y_offset = 0;
	pdf_page = new QLabel;

	NavigationBar *nav_bar = new NavigationBar;

	buildPage(pdf_page, nav_bar);

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), SLOT(scrollUp()));
	connect(nav_bar, SIGNAL(downClick()), SLOT(scrollDown()));
}

PdfPage::~PdfPage()
{
	delete pdf_document;
}

// TODO remove showPage() and use displayPdf() from a file browser
void PdfPage::showPage()
{
	Page::showPage();
	displayPdf("/mnt/sda1/nehe.pdf");
}

void PdfPage::displayPage(int index)
{
	int label_width = pdf_page->width();
	Poppler::Page *page = pdf_document->page(index);

	// scale the page to fit horizontally in the display label
	QSize pdf_size = page->pageSize();
	float ratio = float(pdf_size.width()) / label_width;
	pdf_image = page->renderToImage(SCREEN_DPI / ratio, SCREEN_DPI / ratio);

	delete page;

	y_offset = 0;
	displayPagePart();
}

void PdfPage::displayPagePart()
{
	pdf_page->setPixmap(QPixmap::fromImage(pdf_image.copy(0, y_offset, pdf_page->width(), pdf_page->height())));
}

void PdfPage::displayPdf(const QString &path)
{
	if (pdf_document)
		delete pdf_document;

	pdf_document = Poppler::Document::load(path);
	if (!pdf_document)
		return;

	current_page = 0;
	displayPage(current_page);
}

void PdfPage::scrollDown()
{
	if (y_offset + pdf_page->height() == pdf_image.height() && current_page < pdf_document->numPages() - 1)
	{
		current_page += 1;

		displayPage(current_page);
	}
	else
	{
		y_offset += 50;
		// when at the bottom of the page, the next click will go to the next page
		if (y_offset + pdf_page->height() > pdf_image.height())
			y_offset = pdf_image.height() - pdf_page->height();

		displayPagePart();
	}
}

void PdfPage::scrollUp()
{
	if (y_offset == 0 && current_page > 1)
	{
		current_page -= 1;

		displayPage(current_page);
	}
	else
	{
		y_offset -= 10;
		// when at the top of the page, the next click will go to the previous page
		if (y_offset < 0)
			y_offset = 0;

		displayPagePart();
	}
}
