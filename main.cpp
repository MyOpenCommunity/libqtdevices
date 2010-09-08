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
#ifdef CONFIG_BTOUCH
	QDomElement n = getConfElement("displaypages");
#else
	QDomElement n = getConfElement("gui");
#endif

	if (n.isNull())
		return QDomNode();

	return getChildWithId(n, QRegExp("page(\\d{1,2}|vct|special|menu\\d{1,2}|)"), id);
}

#ifndef CONFIG_BTOUCH
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
	sa.sa_handler = SignalsHandler::signalUSR2Handler;
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
