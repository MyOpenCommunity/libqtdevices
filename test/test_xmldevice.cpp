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
#include "generic_functions.h"


void TestXmlDevice::initTestCase()
{
	qRegisterMetaType<XmlResponse>("XmlResponse");
	dev = new XmlDevice;
}

void TestXmlDevice::cleanupTestCase()
{
	delete dev;
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
	QCOMPARE(dev->pid, 1);
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

void TestXmlDevice::testServerSelectionNoAnswer()
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
				 "			<current_server>no_answer_has_been_received</current_server>"
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::SERVER_SELECTION);
	t.checkError(data, XmlError::SERVER_DOWN);
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
				 "			<status_browse>browse_okay</status_browse>"
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::CHDIR);
	t.check(data, true);
}

void TestXmlDevice::testChdirFail()
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
				 "			<status_browse>server_down</status_browse>"
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::CHDIR);
	t.checkError(data, XmlError::SERVER_DOWN);
}

void TestXmlDevice::testTrackSelection()
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
						 "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\"> "
							 "<item id=\"202\" parentID=\"135\" restricted=\"1\"> "
								 "<dc:title>Morenita</dc:title> "
								 "<upnp:class>object.item.audioItem.musicTrack</upnp:class> "
								 "<upnp:artist>Gloria Estefan</upnp:artist> "
								 "<upnp:album>90 Millas</upnp:album> "
								 "<dc:date>2007-01-01</dc:date> "
								 "<upnp:genre>Pop</upnp:genre> "
								 "<dc:description>Morenita - Gloria Estefan</dc:description> "
								 "<upnp:originalTrackNumber>13</upnp:originalTrackNumber> "
								 "<res protocolInfo=\"http-get:*:audio/mpeg:*\" size=\"6362567\" bitrate=\"25600\" duration=\"0:04:13.000\" sampleFrequency=\"44100\" nrAudioChannels=\"2\">http://10.3.3.245:49152/content/media/object_id/202/res_id/0/ext/file.mp3</res> "
							 "</item> "
						 "</DIDL-Lite> "
				 "		</AW26C2>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::TRACK_SELECTION);
	EntryInfo::Metadata mt;
	mt["title"] = "Morenita";
	mt["artist"] = "Gloria Estefan";
	mt["album"] = "90 Millas";
	mt["total_time"] = "0:04:13.000";
	EntryInfo entry("Morenita", EntryInfo::AUDIO, "http://10.3.3.245:49152/content/media/object_id/202/res_id/0/ext/file.mp3", mt);
	t.check(data, entry);
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
				 "			<status_browse>browse_okay</status_browse>"
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
	t.checkError(data, XmlError::BROWSING);
}

void TestXmlDevice::testSetContextSuccess()
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
				 "		<AW26C16>"
				"		<current_server>MediaTomb</current_server>"
				 "		<status_browse>browse_okay</status_browse>"
				 "		</AW26C16>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::SET_CONTEXT);
	t.check(data, true);
}

void TestXmlDevice::testSetContextFail1()
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
				 "		<AW26C16>"
				 "		<current_server>MediaTomb</current_server>"
				 "		<status_browse>no_such_directory</status_browse>"
				 "		</AW26C16>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::SET_CONTEXT);
	t.checkError(data, XmlError::BROWSING);
}

void TestXmlDevice::testSetContextFail2()
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
				 "		<AW26C16>"
				 "		<current_server>MediaTomb</current_server>"
				 "		<status_browse>server_down</status_browse>"
				 "		</AW26C16>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::SET_CONTEXT);
	t.checkError(data, XmlError::SERVER_DOWN);
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
				 "		<AW26C15>"
				 "			<total>16</total>"
				 "			<rank>1</rank>"
				 "			<directories>"
				 "				<name>TestDirectory1</name>"
				 "				<name>TestDirectory2</name>"
				 "			</directories>"
				 "			<tracks>"
				 "				<file>"
				 "					<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\">"
				 "						<item id=\"13\" parentID=\"0\" restricted=\"1\">"
				 "							<res protocolInfo=\"http-get:*:audio/mpeg:DLNA.ORG_PS=1;DLNA.ORG_CI=0;DLNA.ORG_OP=00;DLNA.ORG_FLAGS=00000000000000000000000000000000;DLNA.ORG_PN=MP3\" size=\"2440262\" duration=\"0:02:29.000\">http://10.3.3.248:49153/files/13</res>"
				 "							<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
				 "							<dc:title>Ship to Monkey Island</dc:title>"
				 "							<upnp:artist>Michael Land</upnp:artist>"
				 "							<upnp:album>The Secret of Monkey Island (game rip)</upnp:album>"
				 "							<upnp:genre>Soundtrack</upnp:genre>"
				 "						</item>"
				 "					</DIDL-Lite>"
				 "				</file>"
				 "				<file>"
				 "					<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:dlna=\"urn:schemas-dlna-org:metadata-1-0/\">"
				 "						<item id=\"1\" parentID=\"0\" restricted=\"1\">"
				 "							<res protocolInfo=\"http-get:*:audio/mpeg:DLNA.ORG_PS=1;DLNA.ORG_CI=0;DLNA.ORG_OP=00;DLNA.ORG_FLAGS=00000000000000000000000000000000;DLNA.ORG_PN=MP3\" size=\"2440262\" duration=\"0:03:19.000\">http://10.3.3.248:49153/files/1</res>"
				 "							<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
				 "							<dc:title>Hammer Smashed Face</dc:title>"
				 "							<upnp:artist>Cannibal Corpse</upnp:artist>"
				 "							<upnp:album>Tomb Of The Mutilated</upnp:album>"
				 "							<upnp:genre>Brutal Metal</upnp:genre>"
				 "						</item>"
				 "					</DIDL-Lite>"
				 "				</file>"
				 "			</tracks>"
				 "		</AW26C15>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::LIST_ITEMS);

	EntryInfo::Metadata mt1;
	mt1["title"] = "Ship to Monkey Island";
	mt1["artist"] = "Michael Land";
	mt1["album"] = "The Secret of Monkey Island (game rip)";
	mt1["total_time"] = "0:02:29.000";

	EntryInfo::Metadata mt2;
	mt2["title"] = "Hammer Smashed Face";
	mt2["artist"] = "Cannibal Corpse";
	mt2["album"] = "Tomb Of The Mutilated";
	mt2["total_time"] = "0:03:19.000";

	UPnpEntryList list;
	list.total = 16;
	list.start = 1;
	list.entries << EntryInfo("TestDirectory1", EntryInfo::DIRECTORY, QString());
	list.entries << EntryInfo("TestDirectory2", EntryInfo::DIRECTORY, QString());
	list.entries << EntryInfo("Ship to Monkey Island", EntryInfo::AUDIO, "http://10.3.3.248:49153/files/13", mt1);
	list.entries << EntryInfo("Hammer Smashed Face", EntryInfo::AUDIO, "http://10.3.3.248:49153/files/1", mt2);
	t.check(data, list);
}

void TestXmlDevice::testListItemsFail()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>46027648-71E9-4FDE-9FE1-E153C907743B</SID>"
				 "			<PID>3</PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.245</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP>10.3.3.98</IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C15>"
				 "			<status_browse>server_down</status_browse>"
				 "		</AW26C15>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::LIST_ITEMS);
	t.checkError(data, XmlError::SERVER_DOWN);
}

void TestXmlDevice::testResetWithAck()
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
				 "		<ACK>"
				 "			<RC>200</RC>"
				 "		</ACK>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::ACK);
	t.check(data, true);
	QVERIFY(dev->sid != QString("1EFC3E00-2066-6C13-55D2-81D7D7DB0E62"));
	QCOMPARE(dev->pid, 0);
}

void TestXmlDevice::testBuildCommand()
{
	QString data("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				 "<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
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
	dev->pid = 1;
	dev->local_addr = "local_address";
	dev->server_addr = "server_address";

	QString command = dev->buildCommand("command");

	QCOMPARE(command, data);
}

void TestXmlDevice::testBuildCommandWithArg()
{
	QString data("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
				 "<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
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
	dev->pid = 1;
	dev->local_addr = "local_address";
	dev->server_addr = "server_address";

	XmlArguments arg;
	arg["id"] = "test_argument";
	QString command = dev->buildCommand("command", arg);

	QCOMPARE(command, data);
}

void TestXmlDevice::testBadXml()
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
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::INVALID);
	t.checkError(data, XmlError::PARSE);
}

void TestXmlDevice::testBadHeader()
{
	QString data("<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\""
				 "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				 "	<Hdr>"
				 "		<MsgID>"
				 "			<SID>1EFC3E00-2066-6C13-55D2-81D7D7DB0E62</SID>"
				 "			<PID></PID>"
				 "		</MsgID>"
				 "		<Dst>"
				 "			<IP>10.3.3.195</IP>"
				 "		</Dst>"
				 "		<Src>"
				 "			<IP></IP>"
				 "		</Src>"
				 "	</Hdr>"
				 "	<Cmd>"
				 "		<AW26C7>"
				 "			<status_browse>browse_ok</status_browse>"
				 "		</AW26C7>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::INVALID);
	t.checkError(data, XmlError::PARSE);
}

void TestXmlDevice::testUnhandledResponse()
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
				 "		<FOOBAR>"
				 "			<foo>browse_ok</foo>"
				 "		</FOOBAR>"
				 "	</Cmd>"
				 "</OWNxml>");

	XmlDeviceTester t(dev, XmlResponses::INVALID);
	t.checkError(data, XmlError::PARSE);
}
