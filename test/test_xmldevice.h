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
#ifndef TESTXMLDEVICE_H
#define TESTXMLDEVICE_H

#include <QObject>

class XmlDevice;


class TestXmlDevice : public QObject
{
Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	void testHeader();
	void testWelcome();
	void testServerList();
	void testServerListEmpty();
	void testServerSelection();
	void testServerSelectionServerDown();
	void testServerSelectionEmpty();
	void testServerSelectionNoAnswer();
	void testChdir();
	void testChdirServerDown();
	void testChdirEmpty();
	void testChdirNotFound();
	void testTrackSelection();
	void testTrackSelectionServerDown();
	void testNextTrack();
	void testNextTrackServerDown();
	void testPrevTrack();
	void testPrevTrackServerDown();
	void testBrowseUpSuccess();
	void testBrowseUpFail();
	void testBrowseUpServerDown();
	void testListItems();
	void testListItemsServerDown();
	void testResetWithAck();
	void testSetContextSuccess();
	void testSetContextFail();
	void testSetContextServerDown();

	void testBuildCommand();
	void testBuildCommandWithArg();

	void testBadXml();
	void testBadHeader();
	void testUnhandledResponse();

private:
	XmlDevice *dev;
};

#endif // TESTXMLDEVICE_H
