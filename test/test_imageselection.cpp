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

void TestImageSelection::initTestCase()
{
	system("touch slideshow_file.txt");
}

void TestImageSelection::cleanupTestCase()
{
	system("rm slideshow_file.txt");
}

void TestImageSelection::testFileLoading()
{
	QStringList images;
	images << "/mnt/disk/img/photo1.jpg"
	       << "/mnt/disk/img/photo2.jpg"
	       << "/mnt/disk/img/photo3.jpg"
	       << "/mnt/disk/image.jpg";

	QTemporaryFile f("./temp_slideshowXXXXXX.txt");
	QVERIFY(f.open());

	QSet<QString> result;

	foreach (const QString &i, images)
	{
		f.write(i.toAscii() + "\n");
		result.insert(i);
	}
	f.close();

	image_handler = new ImageSelectionHandler(f.fileName());
	QCOMPARE(image_handler->getSelectedImages(), result);
}
