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
#include "xmlvarihandler.h"
#include "genericfunz.h"

#include <openwebnet.h>
#include <common_functions.h>

#include <QApplication>
#include <QTranslator>
#include <QVector>
#include <QRegExp>
#include <QtDebug>

#define TIMESTAMP
#ifdef TIMESTAMP
#include <QDateTime>
#endif

#include <signal.h>


// Instance global object to handle icons
IconDispatcher icons_library;


// Instance DOM global object to handle configuration.
QDomDocument qdom_appconfig;

// Configurazione applicativo - path - verbosity - ecc

// Variabili di inizializzazione
char *My_File_Cfg = MY_FILE_CFG_DEFAULT;
char *My_File_User_Cfg = MY_FILE_USER_CFG_DEFAULT;
char *My_File_Log = MY_FILE_LOG_DEFAULT;
char *My_Parser = MY_PARSER_DEFAULT;
char *Xml_File_In = XML_FILE_IN_DEFAULT;
char *Xml_File_Out = XML_FILE_OUT_DEFAULT;
FILE *StdLog = stdout;

// il VERBOSITY_LEVEL condiziona tutte le printf
int  VERBOSITY_LEVEL = VERBOSITY_LEVEL_DEFAULT;
char *Path_Var = NULL;
char *bt_wd = NULL;
char *My_Path = NULL;

// il Suffisso serve per distinguere le stampe
char *Suffisso = "<BTo>";

// Variabili SSL 
int use_ssl = false;
char *ssl_cert_key_path = NULL;
char *ssl_certificate_path = NULL;

BtMain *BTouch;

QDomNode getChildWithName(QDomNode parent, QString name)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName() == name)
			return n;

		n = n.nextSibling();
	}
	return QDomNode();
}

QDomElement getConfElement(QString path)
{
	QStringList sl = path.split('/');
	QDomElement root = qdom_appconfig.documentElement();
	QDomNode node = root;
	while (!sl.isEmpty())
	{
		QString str = sl.first();
		if (!node.isNull())
			node = getChildWithName(node, str);
		sl.pop_front();
	}
	return node.toElement();
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

void readExtraConf(QColor **bg, QColor **fg1, QColor **fg2)
{
	QFile *xmlFile;
	*bg = *fg1 = *fg2 = NULL;

	if (QFile::exists(EXTRA_FILE))
	{
		xmlskinhandler *handler1 = new xmlskinhandler(bg, fg1, fg2);
		xmlFile = new QFile(EXTRA_FILE);
		QXmlInputSource source1(xmlFile);
		QXmlSimpleReader reader1;
		reader1.setContentHandler(handler1);
		reader1.parse(source1);
		delete handler1;
		delete xmlFile;
	}

	if (!*bg)
		*bg = new QColor(77,61,66);

	if (!*fg1)
		*fg1 = new QColor(205,205,205);

	if (!*fg2)
		*fg2 = new QColor(7,151,254);
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
		fprintf(StdLog, "<BTo>%.2d:%.2d:%.2d,%.1d  %s\n",QTime::currentTime().hour() ,QTime::currentTime().minute(),
			QTime::currentTime().second(),QTime::currentTime().msec()/100,msg);
#endif
		break;
	case QtWarningMsg:
		if (VERBOSITY_LEVEL > 0)
			fprintf(StdLog, "<BTo> %s\n", msg);
		break;
	case QtFatalMsg:
	default:
		fprintf(stderr, "<BTo> FATAL %s\n", msg);
		// deliberately core dump
		abort();
	}
}

QDomNode getChildWithId(QDomNode parent, const QRegExp &node_regexp, int id)
{
	QDomNode n = parent.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(node_regexp))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != "id")
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text().toInt() == id)
				return n;
		}
		n = n.nextSibling();
	}
	return QDomNode();
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
	QString default_language(DEFAULT_LANGUAGE);
	QDomNode node = qdom_appconfig.documentElement();

	QVector<QString> node_names(3);
	node_names[0] = "setup";
	node_names[1] = "generale";
	node_names[2] = "language";

	for (QVector<QString>::iterator It = node_names.begin(); It != node_names.end(); ++It)
	{
		node = node.namedItem(*It);
		if (node.isNull())
			return default_language;
	}

	return node.toElement().text();
}


int main(int argc, char **argv)
{
	 // Inizio Lettura configurazione applicativo
	QFile *xmlFile;
	char *logFile;

	logFile = new(char[MAX_PATH]);
	strncpy(logFile, My_File_Log, MAX_PATH);

	QFile file(MY_FILE_USER_CFG_DEFAULT);

	if (!qdom_appconfig.setContent(&file))
	{
		file.close();
		qFatal("Error in qdom_appconfig file, exiting");
	}
	file.close();

	QApplication a(argc, argv);

	xmlcfghandler *handler = new xmlcfghandler(&VERBOSITY_LEVEL, &logFile);
	xmlFile = new QFile(My_File_Cfg);
	QXmlInputSource source(xmlFile);
	QXmlSimpleReader reader;
	reader.setContentHandler(handler);
	reader.parse(source);
	delete handler;
	delete xmlFile;

	if (strcmp(logFile,"") && strcmp(logFile,"-"))  // Settato il file di log
		StdLog = fopen(logFile,"a+");
	if (NULL == StdLog)
	{
		StdLog = stdout;
		qDebug("StdLog==NULL");
	}
	// No bufferizzazione
	setvbuf(StdLog, (char *)NULL, _IONBF, 0);
	setvbuf(stdout, (char *)NULL, _IONBF, 0);
	setvbuf(stderr, (char *)NULL, _IONBF, 0);
	// D'ora in avanti qDebug, ... scrivono dove gli ho detto io
	qInstallMsgHandler(myMessageOutput);

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
	signal(SIGUSR2, ResetTimer);

	qDebug("Start BtMain");
	BTouch = new BtMain(NULL);
	return a.exec();
}
