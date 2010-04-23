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



#include "test_imageselection.h"
#include "imageselectionhandler.h"

#include <QtTest/QTest>
#include <QTemporaryFile>
#include <QDebug>

const QString DIR_PATH = "disk/photos/";
const QString FILE_PATH = "disk/img/photo4.jpg";
const QString COMPLEX_FILE = DIR_PATH + "/tmp/image.jpg";
const QString NOT_PRESENT = "disk/img/not_present.jpg";

void TestImageSelection::initTestCase()
{
	images << "disk/img/photo1.jpg"
	       << "disk/img/photo2.jpg"
	       << "disk/img/photo3.jpg"
	       << "disk/image.jpg"
	       << DIR_PATH
	       << FILE_PATH;

	// TODO: probably all the items in images must be created on file system...

	f = new QTemporaryFile("./temp_slideshowXXXXXX.txt");
	QVERIFY(f->open());

	foreach (const QString &i, images)
		f->write(i.toLocal8Bit().simplified() + "\n");
	f->flush();
}

void TestImageSelection::cleanupTestCase()
{
	f->close();
	delete f;
}

void TestImageSelection::testFileLoading()
{
	QSet<QString> result;
	foreach (const QString &i, images)
		result << i;
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->getSelectedImages(), result);
}

void TestImageSelection::testFileSaving()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	image_handler->selected_images.clear();

	foreach (const QString &i, images)
		image_handler->insertItem(i);
	image_handler->saveSlideshowToFile();

	// now verify correct saving
	// read everything from file then compare it with the set inside image_handler
	QFile test(f->fileName());
	QVERIFY(test.open(QIODevice::ReadOnly));
	QSet<QString> result;
	while (!test.atEnd())
	{
		QString s = test.readLine().simplified();
		result << s;
	}
	QCOMPARE(image_handler->getSelectedImages(), result);
	delete image_handler;
}

void TestImageSelection::testSimplePathSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(FILE_PATH), true);
	delete image_handler;
}

void TestImageSelection::testSimpleDirSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(DIR_PATH), true);
	delete image_handler;
}

void TestImageSelection::testFileSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(COMPLEX_FILE), true);
	delete image_handler;
}

void TestImageSelection::testFileNotSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(NOT_PRESENT), false);
	delete image_handler;
}
