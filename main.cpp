/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** main.cpp
 **
 **Gestione apertura applicazione
 **
 ****************************************************************/

#include "main.h"
#include "btmain.h"
#include "xml_functions.h"
#include "generic_functions.h"

#include <common_functions.h>

#include <QApplication>
#include <QTranslator>
#include <QVector>
#include <QtDebug>
#include <QFile>

#define TIMESTAMP
#ifdef TIMESTAMP
#include <QDateTime>
#endif

#include <signal.h>


/// The struct that contain the global configuration values
struct GlobalConfig
{
	int verbosity_level;
	QString log_file;
};


// Instance DOM global object to handle configuration.
QDomDocument qdom_appconfig;

// Instance of a global config
GlobalConfig global_config;

// A global pointer to the log file
FILE *StdLog = stdout;

// The global verbosity_level, used by BTouch and libcommon (MySignal)
int VERBOSITY_LEVEL;

// Only for the linking with libcommon
char *My_Parser = MY_PARSER_DEFAULT;
char *Suffisso = "<BTo>";
int use_ssl = false;
char *ssl_cert_key_path = NULL;
char *ssl_certificate_path = NULL;

/// A global pointer to BtMain
BtMain *BTouch;


QDomElement getConfElement(QString path)
{
	return getElement(qdom_appconfig.documentElement(), path);
}

TemperatureScale readTemperatureScale()
{
	TemperatureScale default_scale = CELSIUS;

	static TemperatureScale scale = NONE;
	// cache the value
	if (scale != NONE)
		return scale;

	QDomElement temperature_format = getConfElement("setup/generale/temperature/format");
	if (temperature_format.isNull())
	{
		qWarning("Temperature scale not found on conf.xml!");
		scale = default_scale;
	}
	else
	{
		scale = static_cast<TemperatureScale>(temperature_format.text().toInt());
	}

	return scale;
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
	case QtFatalMsg:
	default:
		fprintf(stderr, "<BTo> FATAL %s\n", msg);
		// deliberately core dump
		abort();
	}
}

QDomNode getPageNode(int id)
{
	QDomElement root = qdom_appconfig.documentElement();

	QDomNode n = root.firstChild();
	while (!n.isNull() && n.nodeName() != "displaypages")
		n = n.nextSibling();

	if (n.isNull())
		return QDomNode();

	return getChildWithId(n, QRegExp("page\\d{1,2}"), id);
}

QString getLanguage()
{
	QDomElement l = getConfElement("setup/generale/language");
	if (!l.isNull())
		return l.text();

	return QString(DEFAULT_LANGUAGE);
}

static void loadGlobalConfig(QString xml_file)
{
	global_config.verbosity_level = VERBOSITY_LEVEL_DEFAULT;
	global_config.log_file = MY_FILE_LOG_DEFAULT;

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
					global_config.verbosity_level = v.text().toInt();

				QDomNode l = getChildWithName(el, "logfile");
				if (!l.isNull())
					global_config.log_file = l.toElement().text();
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


int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	QFile file(MY_FILE_USER_CFG_DEFAULT);
	if (!qdom_appconfig.setContent(&file))
	{
		file.close();
		qFatal("Error in qdom_appconfig file, exiting");
	}
	file.close();

	loadGlobalConfig(MY_FILE_CFG_DEFAULT);
	setupLogger(global_config.log_file);
	VERBOSITY_LEVEL = global_config.verbosity_level;

	QString language_suffix = getLanguage();
	if (language_suffix != QString(DEFAULT_LANGUAGE))
	{
		QString language_file;
		language_file.sprintf(LANGUAGE_FILE_TMPL, language_suffix.toAscii().constData());
		QTranslator *translator = new QTranslator(0);
		if (translator->load(language_file))
			a.installTranslator(translator);
		else
			qWarning() << "File " << language_file << " not found for language " << language_suffix;
	}

	// Fine Lettura configurazione applicativo
	signal(SIGUSR1, MySignal);
	signal(SIGUSR2, resetTimer);

	qDebug("Start BtMain");
	BTouch = new BtMain(NULL);
	return a.exec();
}
