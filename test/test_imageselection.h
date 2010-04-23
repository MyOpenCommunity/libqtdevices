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



#ifndef TEST_IMAGESELECTION_H
#define TEST_IMAGESELECTION_H

#include <QObject>
#include <QStringList>

class ImageSelectionHandler;
class QTemporaryFile;

class TestImageSelection : public QObject
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void testFileLoading();
	void testFileSaving();

	// file explicitly present in the set
	void testSimplePathSelected();

	// directory explicitly present in the set
	void testSimpleDirSelected();

	// file included in a selected directory
	void testFileSelected();

	// file non present
	void testFileNotSelected();

private:
	ImageSelectionHandler *image_handler;
	QStringList images;
	QTemporaryFile *f;
};

#endif // TEST_IMAGESELECTION_H
