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
#include "test_xmldevice.h"

#include <QtTest>

#include "xmldevice.h"
#include "xmldevice_tester.h"

TestXmlDevice::TestXmlDevice() :
	QObject(), dev(new XmlDevice)
{
	qRegisterMetaType<XmlResponse>("XmlResponse");
}

void TestXmlDevice::testHeader()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
					"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
					"	<Hdr>"
					"		<MsgID>"
					"			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
					"			<PID>1</PID>"
					"		</MsgID>"
					"		<Dst>"
					"			<IP>10.3.3.195</IP>"
					"		</Dst>"
					"		<Src>"
					"			<IP>192.168.1.110</IP>"
					"		</Src>"
					"	</Hdr>"
					"	<Cmd Dsc=\"Welcome Message\">"
					"		<WMsg>"
					"			<PwdOp>672295445</PwdOp>"
					"		</WMsg>"
					"	</Cmd>"
					"</OWNxml>");

	dev->handleData(data);

	QCOMPARE(dev->sid, QString("1EFC3E00-2066-6C13-55D2-81D7D7DB0E62"));
	QCOMPARE(dev->pid, QString("1"));
	QCOMPARE(dev->local_addr, QString("10.3.3.195"));
	QCOMPARE(dev->server_addr, QString("192.168.1.110"));
}

void TestXmlDevice::testWelcome()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
					"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
					"	<Hdr>"
					"		<MsgID>"
					"			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
					"			<PID>0</PID>"
					"		</MsgID>"
					"		<Dst>"
					"			<IP>10.3.3.195</IP>"
					"		</Dst>"
					"		<Src>"
					"			<IP>192.168.1.110</IP>"
					"		</Src>"
					"	</Hdr>"
					"	<Cmd Dsc=\"Welcome Message\">"
					"		<WMsg>"
					"			<PwdOp>672295445</PwdOp>"
					"		</WMsg>"
					"	</Cmd>"
					"</OWNxml>");

	QSignalSpy spy(dev, SIGNAL(responseReceived(XmlResponse)));
	dev->handleData(data);

	QCOMPARE(spy.count(), 1);

	QList<QVariant> arguments = spy.takeFirst();

	XmlResponse response = arguments.at(0).value<XmlResponse>();

	QVERIFY(response.contains(XmlResponses::WELCOME));
}

void TestXmlDevice::testServerList()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C1>"
				 "			<server>"
				 "				<name>TestServer1</name>"
				 "				<name>TestServer2</name>"
				 "			</server>"
				 "		</AW26C1>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::SERVER_LIST);
	t.check(data, QStringList() << "TestServer1" << "TestServer2");
}

void TestXmlDevice::testServerSelection()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C2>"
				 "			<current_server>TestServer</current_server>"
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::SERVER_SELECTION);
	t.check(data, QString("TestServer"));
}

void TestXmlDevice::testChdir()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C2>"
				 "			<status_browse>browse_ok</status_browse>"
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::CHDIR);
	t.check(data, true);
}

void TestXmlDevice::testSelectTrack()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C2>"
				 "			 <DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\">"
				 "				<item id=\"13\" parentID=\"0\" restricted=\"1\">"
				 "					<res protocolInfo=\"http-get:*:audio/mpeg:DLNA.ORG_PS=1;DLNA.ORG_CI=0;DLNA.ORG_OP=00;DLNA.ORG_FLAGS=00000000000000000000000000000000;DLNA.ORG_PN=MP3\" size=\"2440262\" duration=\"0:02:29.000\">http://10.3.3.248:49153/files/13</res>"
				 "					<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
				 "					<dc:title>Ship to Monkey Island</dc:title>"
				 "					<upnp:artist>Michael Land</upnp:artist>"
				 "					<upnp:album>The Secret of Monkey Island (game rip)</upnp:album>"
				 "					<upnp:genre>Soundtrack</upnp:genre>"
				 "				</item>"
				 "			</DIDL-Lite>"
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::TRACK_SELECTION);
	t.check(data, QString("http://10.3.3.248:49153/files/13"));
}

void TestXmlDevice::testBrowseUpSuccess()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C7>"
				 "			<status_browse>browse_ok</status_browse>"
				 "		</AW26C7>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::BROWSE_UP);
	t.check(data, true);
}

void TestXmlDevice::testBrowseUpFail()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C7>"
				 "			<status_browse>already_at_root</status_browse>"
				 "		</AW26C7>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::BROWSE_UP);
	t.check(data, false);
}

void TestXmlDevice::testListItems()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID>4</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>192.168.1.110</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C6>"
				 "			<directories>"
				 "				<name>TestDirectory1</name>"
				 "				<name>TestDirectory2</name>"
				 "			</directories>"
				 "			<tracks>"
				 "				<name>TestTrack1</name>"
				 "				<name>TestTrack2</name>"
				 "			</tracks>"
				 "		</AW26C6>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::LIST_ITEMS);
	t.check(data, FilesystemEntries() <<
				  FilesystemEntry("TestDirectory1", FilesystemEntry::DIRECTORY) <<
				  FilesystemEntry("TestDirectory2", FilesystemEntry::DIRECTORY) <<
				  FilesystemEntry("TestTrack1", FilesystemEntry::TRACK) <<
				  FilesystemEntry("TestTrack2", FilesystemEntry::TRACK));
}

void TestXmlDevice::testBuildCommand()
{
	QString data("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				 "<OWNxml xmlns=\"http://www.bticino.it/xopen/v1 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
				 "	<Hdr>\n"
				 "		<MsgID>\n"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>\n"
				 "			<PID>1</PID>\n"
				 "		</MsgID>\n"
				 "		<Dst>\n"
				 "			<IP>server_address</IP>\n"
				 "		</Dst>\n"
				 "		<Src>\n"
				 "			<IP>local_address</IP>\n"
				 "		</Src>\n"
				 "	</Hdr>\n"
				 "	<Cmd>\n"
				 "		<command/>\n"
				 "	</Cmd>\n"
				 "</OWNxml>\n");

	dev->sid = "1EFC3E00-2066-6C13-55D2-81D7D7DB0E62";
	dev->pid = "1";
	dev->local_addr = "local_address";
	dev->server_addr = "server_address";

	QString command = dev->buildCommand("command");

	QCOMPARE(command, data);
}

void TestXmlDevice::testBuildCommandWithArg()
{
	QString data("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				 "<OWNxml xmlns=\"http://www.bticino.it/xopen/v1 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
				 "	<Hdr>\n"
				 "		<MsgID>\n"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>\n"
				 "			<PID>1</PID>\n"
				 "		</MsgID>\n"
				 "		<Dst>\n"
				 "			<IP>server_address</IP>\n"
				 "		</Dst>\n"
				 "		<Src>\n"
				 "			<IP>local_address</IP>\n"
				 "		</Src>\n"
				 "	</Hdr>\n"
				 "	<Cmd>\n"
				 "		<command>\n"
				 "			<id>test_argument</id>\n"
				 "		</command>\n"
				 "	</Cmd>\n"
				 "</OWNxml>\n");

	dev->sid = "1EFC3E00-2066-6C13-55D2-81D7D7DB0E62";
	dev->pid = "1";
	dev->local_addr = "local_address";
	dev->server_addr = "server_address";

	QString command = dev->buildCommand("command", "test_argument");

	QCOMPARE(command, data);
}
