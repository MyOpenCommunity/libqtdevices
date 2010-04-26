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
#include <QDir>

const QString DIR_PATH = "disk/photos";
QString FILE_PATH;
const QString COMPLEX_FILE = DIR_PATH + "/tmp/image.jpg";
const QString NOT_PRESENT = "disk/img/not_present.jpg";

void TestImageSelection::initTestCase()
{
	// create paths on disk, to be used with compactDirectory()
	// paths created are:
	QDir d(".");
	d.mkpath("disk/img");

	// disk/img/photo[1-4].jpg
	for (int i = 1; i < 5; ++i)
	{
		QString path = QString("disk/img/photo%1.jpg").arg(i);
		QFile f(path);
		f.open(QIODevice::WriteOnly);
		f.close();
		images << path;
	}
	FILE_PATH = "disk/img/photo4.jpg";

	// disk/photos/
	images << DIR_PATH;

	f = new QTemporaryFile("./temp_slideshowXXXXXX.txt");
	QVERIFY(f->open());

	foreach (const QString &i, images)
	{
		// file saved must be full path names
		QString path(QFileInfo(i).absoluteFilePath());
		f->write(path.toLocal8Bit().simplified() + "\n");
	}
	f->flush();
}

void TestImageSelection::cleanupTestCase()
{
	// cleanup directory before deleting it
	foreach (const QFileInfo &fi, QDir("disk/img").entryInfoList())
		QFile::remove(fi.canonicalFilePath());

	QDir cleanup(".");
	cleanup.rmpath("disk/img");
	f->close();
	delete f;
}

void TestImageSelection::testFileLoading()
{
	QSet<QString> result;
	foreach (const QString &i, images)
	{
		QString path(QFileInfo(i).absoluteFilePath());
		result << path;
	}
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->getSelectedImages(), result);
	delete image_handler;
}

void TestImageSelection::testFileSaving()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	image_handler->selected_images.clear();

	foreach (const QString &i, images)
		image_handler->insertItem(QFileInfo(i).absoluteFilePath());
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
	QCOMPARE(image_handler->isItemSelected(QFileInfo(FILE_PATH).absoluteFilePath()), true);
	delete image_handler;
}

void TestImageSelection::testSimpleDirSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(QFileInfo(DIR_PATH).absoluteFilePath()), true);
	delete image_handler;
}

void TestImageSelection::testFileSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(QFileInfo(COMPLEX_FILE).absoluteFilePath()), true);
	delete image_handler;
}

void TestImageSelection::testFileNotSelected()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	QCOMPARE(image_handler->isItemSelected(QFileInfo(NOT_PRESENT).absoluteFilePath()), false);
	delete image_handler;
}

void TestImageSelection::testCompactDirectory()
{
	image_handler = new ImageSelectionHandler(f->fileName());
	// trigger compactDirectory()
	image_handler->removeItem(QFileInfo(FILE_PATH).absoluteFilePath());
	image_handler->insertItem(QFileInfo(FILE_PATH).absoluteFilePath());
	image_handler->setFileFilter(QStringList() << "*.jpg");

	QCOMPARE(image_handler->isItemSelected(QFileInfo("disk/img").absoluteFilePath()), true);
	delete image_handler;
}
