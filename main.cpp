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


#include "btmain.h" // bt_global::btmain, SignalsHandler
#include "xml_functions.h"
#include "generic_functions.h"

#include <common_functions.h>

#include <QApplication>
#include <QTranslator>
#include <QWSServer>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QDomNode>
#include <QString>

#define TIMESTAMP
#ifdef TIMESTAMP
#include <QDateTime>
#endif

#include <signal.h>

/*!
	\mainpage

	\section overview A global overview of the User Interface
	BTouch is a touchscreen user interface for MyHome, BTicino home automation system;
	it has a key role in the control of all its functionalities.

	BTouch can run in 3.5'' and 10'' touchscreens.  It's possible
	to test the user interface on a Linux PC; see \ref page-x86 for caveats
	and limitations.

	\section overview-architecture System architecture

	The base of the system is a customized GNU/Linux distribution; the BTicino-specific parts are:
	\li subsystem-specific processes
	\li BTouch (the GUI)
	\li OpenServer

	The OpenServer is the front-end to the home automation system: it provides an OpenWebNet interface
	to communicate with the various physical devices (the actual communication is performed by the
	subsystem-specific processes).

	Inside the GUI, the communication with physical devices is abstracted by
	\ref device subclasses that avoid the need to deal with OpenWebNet directly.

	See the \ref page-gui to an explaination of how the GUI works.

	\section modifying-gui Modifying the GUI

	bla bla bla
	See the \ref page-examples.
*/

/*!
	\page page-examples BTouch examples

	The examples below demostrate how to modify the source code of BTouch in order to add
	new functionalities or change the default behaviour. The examples can be included in the BTouch
	executable by running qmake as:
	\code
	qmake CONFIG += examples
	\endcode

	The available examples are:
	- \ref page-example-pdf
	- \ref page-example-screensaver
	- \ref page-example-message
*/

/*!
	\page page-example-pdf The pdf viewer example

	We want to add a new subsection of the multimedia section to navigate
	a mounted file system (USB or SD) and display the contained PDF files.

	LICENSE NOTE: this example uses the Poppler library, which is GPL, hence
	the resulting binary must be redistributed under GPL terms.

	To compile the example you need to recompile the Poppler library: detailed
	instructions are in \c examples/pdf/README.txt; after compiling the example,
	set \c BUILD_PDF to \c yes in \c examples.pri.

	First, we need a page with a standard layout (navigation bar on the left, display
	area on the right):
	\dontinclude pdfpage.cpp
	\skip PdfPage::PdfPage(
	\until }

	displayPdf() is called by the file system navigation page to load and display
	a PDF file.
	\skip PdfPage::displayPdf(
	\until }

	displayPage() renders the specified page into a QImage large as the display area
	and displays it into the page content.
	\skip PdfPage::displayPage(
	\until }

	scrollDown() scrolls down the displayed page; when at the bottom of the page,
	it moves to the next page.
	\skip PdfPage::scrollDown(
	\until }
	\until }
	\until }

	The new configuration node does not need any additional parameters; we only need to
	choose a non-conflicting id for the new item; the \c cid is chosen to use the
	existing USB icon; any \c cid defined in \c skin.xml will work:
	\verbatim
      <item>
	<square>0</square>
	<id>55556</id>
	<cid>16002</cid>
	<itemID>5745934</itemID>
	<descr>PDF</descr>
      </item>
	\endverbatim

	The configuration item is parsed in the \ref Multimedia section page;
	first we define a new page id:
	\dontinclude multimedia.cpp
	\skip #ifdef BUILD_EXAMPLES
	\until #endif

	that is used in the MultimediaSectionPage::loadItems() method:
	\skip case PAGE_PDF:
	\until }
	\until }
	this example only browses an USB file system; use MOUNT_SD in the code
	above to browse an SD card.

	There are some other parts that need to be modified; the PDF type must be
	defined in \c generic_functions.h
	\dontinclude generic_functions.h
	\skip #ifdef BUILD_EXAMPLES
	\until #endif

	and \c generic_functions.cpp must be modified accordingly:
	\dontinclude generic_functions.cpp
	\skip const char *image_files
	\until #endif

	to add PDF file handling in getFileExtensions()
	\skip #ifdef BUILD_EXAMPLES
	\until #endif

	and in getFileFilter()
	\skip #ifdef BUILD_EXAMPLES
	\until #endif

	The last class that needs changes is MultimediaFileListPage; first we need
	to add the icon for PDF files to the icon list:
	\dontinclude multimedia_filelist.cpp
	\skip #ifdef PDF_EXAMPLE
	\skip #endif
	\skip #ifdef PDF_EXAMPLE
	\until #endif

	then we instantiate the PDF display page and set up signals so the file list
	is shown again when the PDF page is closed
	\skip #ifdef PDF_EXAMPLE
	\until #endif

	we add a new case for PDF files in MultimediaFileListPage::fileType()
	\skip #ifdef PDF_EXAMPLE
	\until #endif

	and finally we emit the displayPdf() signal when a PDF file is clicked (the signal
	is connected to the displayPdf() slot of PdfPage.
	\skip #ifdef PDF_EXAMPLE
	\until #endif
*/

/*!
	\page page-example-screensaver The logo screeensaver example

	We want to add a screensaver that shows a bouncing myhome logo.

	The first thing to do is subclass the abstract class ScreenSaver.
	As described in the documentation of the class, we have to reimplement the
	ScreenSaver::type() and ScreenSaver::refresh() methods.

	To show a MyHome logo moving on the screen, we redefine
	the constructor to get and save the image from the SkinManager:
	\dontinclude logoscreensaver.cpp
	\skip LogoScreenSaver::LogoScreenSaver()
	\until }

	Now we have to place the logo on the screen. Because we have to re-place
	the image every time that the screensaver starts, we redefine the
	ScreenSaver::start() method:
	\skip LogoScreenSaver::start
	\until }

	Note that the LogoScreenSaver class explicitly calls the Window::showWindow()
	method. This is required because some screensavers (like the ScreenSaverLine)
	doen't show a window but instead draws over another window.

	The positioning is done by the method initLogoRect():
	\skip LogoScreenSaver::initLogoRect
	\until }

	The paintEvent() method, implicitly called by Qt, draws the
	logo using its current position:

	\skip LogoScreenSaver::paintEvent
	\until }

	Now, we have to change the current position of the logo, redefining the
	ScreenSaver::refresh() method:
	\skip LogoScreenSaver::refresh
	\until }

	The next step is to assign a proper type to our screensaver, adding the type
	to the ScreenSaver::Type enumeration and reimplementing the type() method:
	\skip LogoScreenSaver::type
	\until }

	The only thing left is to allow the user to choose the logo screensaver.
	We do that by adding a new CheckableBanner in the ScreenSaverPage
	constructor, associating it with the ScreenSaver::Type just added
	\dontinclude screensaverpage.cpp
	\skip #ifdef BUILD_EXAMPLES
	\until #endif
*/

/*!
	\page page-example-message Boh
*/

/*!
	\page page-x86 BTouch on Linux PC

	BTouch can be run on Linux PCs (tested on 32-bit Linux, should work, but has not been tested,
	on 64-bit Linux and other Unix variants).

	To test under Linux you need:
	- Qt4 embedded for your architecture
	- qvfb (can be built from Qt sources, but should be packaged in all distributions)
	- a network-connected touchscreen
	- BTicino common library for your architecture

	Use the same build procedure documented for ARM cross-compilation (using the correct Qt embedded build and toolchain).

	The GUI needs to connect to an OpenServer: you need to enable external connections in the configuration file (\c abilita_ip
	node, under \c sicurezza).

	Before running BTouch, launch qvfb and select the appropriate resolution.

	To run BTouch use "LD_LIBRARY_PATH=/path/to/libcommon ./BTouch.x86 -qws"; you should see the interface displayed inside qvfb.

	Most functions work without problems when run on PC; what does not work (or works only partially):
	- calibration is not implemented
	- using the PC as a sound diffusion source/amplifier is not possible
	- mplayer must be installed for local audio playback (\ref Multimedia section)
	- local video playback (\ref Multimedia section) works but the video is displayed
		full-screen, not in the BTouch interface
	- screen brightness does not change in freeze/screensaver mode
	- screen is not turned off after the blank screen timeout
	- state of videocalls can be controlled, but audio/video is not shown
	- the system should allow navigating an USB key from the multimedia section
	  but will not mount/umount it
*/

/*!
	\page page-gui GUI Overview

	The top level GUI component is WindowContainer which manages all full-screen pages (password
	keypad, screensaver, main application window, ...) all of which are Window subclasses.

	HomeWindow is the main application page and contains HeaderWidget (containing the navigation bar
	and the main page toolbars), FavoritesWidget (the lateral favourites column) and PageContainer
	(containing the application pages).

	In TS 3.5'' the basic structure is the same, but HeaderWidget and FavoritesWidget are not instantiated.

	\section modules Modules

	The GUI is logically composed by modules. Usually a module identifies a
	section or application, but in general is a group of code that work together.
	The list of the current modules (and submodules) is:

	- \ref Antintrusion
	- \ref Automation
	- \ref Lighting
	- \ref EnergyManagement
	- \ref EnergyData
	- \ref Supervision
		- \ref StopAndGo
		- \ref LoadDiagnostic
	- \ref LoadManagement
	- \ref SoundDiffusion
	- \ref VideoDoorEntry
	- \ref Scenarios
	- \ref ThermalRegulation
		- \ref AirConditioning
	- \ref Messages
	- \ref Multimedia
	- \ref Core
	- \ref Settings

	\section overview-page Page overview

	Page is the base class for all content pages; most pages derive from BannerPage (for pages containing banners),
	IconPage (for TS 10'' pages containing icons arranged in a grid).  It's also possible to derive directly from Page
	when a more free-form layout is needed.

	Using methods provided by Page, it's easy to write code that runs unchanged in both TS 3.5'' and TS 10'' (see NavigationBar
	and Page::buildPage()).

	Banners are the building blocks for most sections; in general terms a \ref banner is a rectangular widget that can be
	used to present the status of a device, to interact with the device and to navigate to other pages.  Banners can be used
	in isolation or presented in a menu using BannerPage.

	\section overview-parsing Configuration parsing

	Configuration parsing is handled recursively by GUI classes: global configuration parsing is handled in BtMain; configuration
	parsing for the home page is handled by SectionPage which delegates the parsing of subsections to the GUI classes of the subsection
	itself.

	For sections that only contain banners (\ref Lighting, \ref Automation, \ref Scenarios, \ref Settings), ::getBanner() can be used
	to obtain a banner instance given the configuration node.

	\section overview-test Unit tests

	There are extensive unit test for \ref device and its subclasses (see "devices/test" subdirectory); some non-GUI
	classes also have unit tests (see "test" subdirectory).  Use the execute_test.sh script to run the tests.
*/


/// The struct that contain the general configuration values
struct GeneralConfig
{
	int verbosity_level; // the verbosity used to print in the log file
	QString log_file; // the log filename
	int openserver_reconnection_time; // the timeout after that if the connection is down the UI try to establish again a connection
};

// Instance DOM global object to handle configuration.
// TODO this can be freed after configuration loading completes
QDomDocument qdom_appconfig;

QDomElement getConfElement(QString path)
{
	return getElement(qdom_appconfig.documentElement(), path);
}

void myMessageOutput(QtMsgType type, const char *msg)
{
	switch (type)
	{
	case QtDebugMsg:
		if (VERBOSITY_LEVEL > 1)
#ifndef TIMESTAMP
			fprintf(StdLog, "<BTo> %s\n", msg);
#else
		{
			QTime now = QTime::currentTime();
			fprintf(StdLog, "<BTo>%.2d:%.2d:%.2d,%.1d  %s\n",now.hour(), now.minute(),
				now.second(), now.msec()/100, msg);
		}
#endif
		break;
	case QtWarningMsg:
		if (VERBOSITY_LEVEL > 0)
			fprintf(StdLog, "<BTo> WARNING %s\n", msg);
		break;
	case QtCriticalMsg:
		fprintf(stderr, "<BTo> Critical: %s\n", msg);
		break;
	case QtFatalMsg:
	default:
		fprintf(stderr, "<BTo> FATAL %s\n", msg);
		// deliberately core dump
		abort();
	}
}

QDomNode getPageNode(int id)
{
#ifdef CONFIG_TS_3_5
	QDomElement n = getConfElement("displaypages");
#else
	QDomElement n = getConfElement("gui");
#endif

	if (n.isNull())
		return QDomNode();

	return getChildWithId(n, QRegExp("page(\\d{1,2}|vct|special|menu\\d{1,2}|)"), id);
}

#ifndef CONFIG_TS_3_5
QDomNode getPageNodeFromPageId(int pageid)
{
	QDomElement gui = getConfElement("gui");
	QDomNode page = getChildWithId(gui, QRegExp("page"), "pageID", pageid);

	return page;
}

QDomNode getPageNodeFromChildNode(QDomNode n, QString child_name)
{
	int page_id = getTextChild(n, child_name).toInt();
	QDomNode page_node = getPageNodeFromPageId(page_id);

	return page_node;
}

QDomNode getHomepageNode()
{
	// TODO read the id from the <homepage> node
	return getPageNodeFromPageId(1);
}
#endif

static void loadGeneralConfig(QString xml_file, GeneralConfig &general_config)
{
	general_config.verbosity_level = VERBOSITY_LEVEL_DEFAULT;
	general_config.log_file = MY_FILE_LOG_DEFAULT;
	general_config.openserver_reconnection_time = 30;

	if (QFile::exists(xml_file))
	{
		QFile fh(xml_file);
		QDomDocument qdom_config;
		if (qdom_config.setContent(&fh))
		{
			QDomNode el = getElement(qdom_config, "root/sw");
			if (!el.isNull())
			{
				QDomElement v = getElement(el, "BTouch/logverbosity");
				if (!v.isNull())
					general_config.verbosity_level = v.text().toInt();

				QDomElement r = getElement(el, "BTouch/reconnectiontime");
				if (!r.isNull())
					general_config.openserver_reconnection_time = r.text().toInt();

				QDomNode l = getChildWithName(el, "logfile");
				if (!l.isNull())
					general_config.log_file = l.toElement().text();
			}
		}
	}
}

static void setupLogger(QString log_file)
{
	if (!log_file.isEmpty())
		StdLog = fopen(log_file.toAscii().constData(), "a+");

	if (NULL == StdLog)
		StdLog = stdout;

	// Prevent buffering
	setvbuf(StdLog, (char *)NULL, _IONBF, 0);
	setvbuf(stdout, (char *)NULL, _IONBF, 0);
	setvbuf(stderr, (char *)NULL, _IONBF, 0);

	qInstallMsgHandler(myMessageOutput);
}

void installTranslator(QApplication &a, QString language_suffix)
{
	QString language_file;
	language_file.sprintf(LANGUAGE_FILE_TMPL, language_suffix.toAscii().constData());
	QTranslator *translator = new QTranslator(0);
	if (translator->load(language_file))
		a.installTranslator(translator);
	else
		qWarning() << "File " << language_file << " not found for language " << language_suffix;
}

int main(int argc, char **argv)
{
	GeneralConfig general_config;
	QApplication a(argc, argv);
	QWSServer::setCursorVisible(false);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // force the locale as UTF8

#if DEBUG
	printf("File path: %s\n", qPrintable(a.applicationFilePath()));
#endif

	QFile file(MY_FILE_USER_CFG_DEFAULT);
	if (!qdom_appconfig.setContent(&file))
	{
		file.close();
		qFatal("Unable to load the configuration file: %s, exiting", MY_FILE_USER_CFG_DEFAULT);
	}
	file.close();

	loadGeneralConfig(MY_FILE_CFG_DEFAULT, general_config);
	setupLogger(general_config.log_file);
	VERBOSITY_LEVEL = general_config.verbosity_level;

	// Fine Lettura configurazione applicativo
	signal(SIGUSR1, MySignal);
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_flags |= SA_RESTART;
	sa.sa_handler = SignalsHandler::signalHandler;
	if (sigaction(SIGUSR2, &sa, 0) != 0)
		qWarning() << "Error on installing the handler for the SIGUSR2 signal";
	if (sigaction(SIGTERM, &sa, 0) != 0)
		qWarning() << "Error on installing the handler for the SIGTERM signal";

	SignalsHandler *sh = new SignalsHandler;

	qDebug("Start BtMain");
	bt_global::btmain = new BtMain(general_config.openserver_reconnection_time);
	sh->connect(sh, SIGNAL(signalReceived(int)), bt_global::btmain, SLOT(handleSignal(int)));
	installTranslator(a, (*bt_global::config)[LANGUAGE]);
	int res = a.exec();
	delete bt_global::btmain;
	delete sh;
	return res;
}
