/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** main.cpp
 **
 **Gestione apertura applicazione
 **
 ****************************************************************/

#include "btmain.h"
#include "propertymap_xml.h"
#include <qapplication.h>
#include "../bt_stackopen/common_files/openwebnet.h"
#include "../bt_stackopen/common_files/common_functions.h"
#include "xmlvarihandler.h"
#include <signal.h>
#include <qregexp.h>

#define	TIMESTAMP
#ifdef TIMESTAMP
#include <qdatetime.h>
#endif

#include "main.h"

/*******************************************
 ** Instance global object to handle icons
 *******************************************/
IconDispatcher icons_library;


/*******************************************
 ** Instance global object to handle conf.xml
 *******************************************/
PropertyMap app_config;

/*******************************************
 * Instance DOM global object to handle
 * configuration. This will eventually
 * replace app_config
 * ****************************************/
QDomDocument qdom_appconfig;


/*******************************************
 ** Configurazione applicativo - path - verbosity - ecc
 *******************************************/
// Variabili di inizializzazione
char * My_File_Cfg=MY_FILE_CFG_DEFAULT;
char * My_File_User_Cfg=MY_FILE_USER_CFG_DEFAULT;
char * My_File_Log=MY_FILE_LOG_DEFAULT;
char * My_Parser=MY_PARSER_DEFAULT;
char * Xml_File_In=XML_FILE_IN_DEFAULT;
char * Xml_File_Out=XML_FILE_OUT_DEFAULT;
FILE * StdLog = stdout;
// il VERBOSITY_LEVEL condiziona tutte le printf
int  VERBOSITY_LEVEL=VERBOSITY_LEVEL_DEFAULT;
char * Path_Var=NULL;
char * bt_wd=NULL;
char * My_Path=NULL;
// il Suffisso serve per distinguere le stampe
char * Suffisso = "<BTo>";

// Variabili SSL 
int use_ssl = false;
char *ssl_cert_key_path = NULL;
char *ssl_certificate_path = NULL;

void myMessageOutput( QtMsgType type, const char *msg )
{
	switch ( type ) 
	{
	case QtDebugMsg:
		if (VERBOSITY_LEVEL>1)
		#ifndef TIMESTAMP
		fprintf( StdLog, "<BTo> %s\n", msg );
		#endif
		#ifdef TIMESTAMP
		fprintf( StdLog, "<BTo>%.2d:%.2d:%.2d,%.1d  %s\n",QTime::currentTime().hour() ,QTime::currentTime().minute() ,QTime::currentTime().second(),QTime::currentTime().msec()/100,msg );
		#endif
		break;
	case QtWarningMsg:
		if (VERBOSITY_LEVEL>0)
		fprintf( StdLog, "<BTo> %s\n", msg );
		break;
	case QtFatalMsg:
		fprintf( stderr, "<BTo> FATAL %s\n", msg );
		// deliberately core dump
		abort();
	}
}

QDomNode getPageNode(int id)
{
	QString node_id;
	node_id.setNum(id);
	QDomElement root = qdom_appconfig.documentElement();

	QDomNode n = root.firstChild();
	while (!n.isNull() && n.nodeName() != "displaypages")
		n = n.nextSibling();

	if (n.isNull())
		return QDomNode();

	n = n.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(QRegExp("page\\d{1,2}")))
		{
			QDomNode child = n.firstChild();
			while (!child.isNull() && child.nodeName() != "id")
				child = child.nextSibling();

			if (!child.isNull() && child.toElement().text() == node_id)
				return n;
		}
		n = n.nextSibling();
	}
	return QDomNode();
}

QString getLanguage()
{
	QString default_language;
	QDomNode node = qdom_appconfig.documentElement();

	QValueVector<QString> node_names(3);
	node_names[0] = "setup";
	node_names[1] = "generale";
	node_names[2] = "language";

	for (QValueVector<QString>::iterator It = node_names.begin(); It != node_names.end(); ++It)
	{
		node = node.namedItem(*It);
		if (node.isNull())
			return default_language;
	}

	return node.toElement().text();
}

BtMain *BTouch;

int main( int argc, char **argv )
{
	/*******************************************
	 ** Inizio Lettura configurazione applicativo
	 *******************************************/

	QFile * xmlFile;
	char *logFile;

	logFile=new(char[MAX_PATH]);
	strncpy(logFile, My_File_Log, MAX_PATH);

	// load configuration from conf.xml to app_config
	propertyMapLoadXML( app_config, MY_FILE_USER_CFG_DEFAULT );
	
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
	QXmlInputSource source( xmlFile );
	QXmlSimpleReader reader;
	reader.setContentHandler( handler );
	reader.parse( source );
	delete handler;
	delete xmlFile;

	if (strcmp(logFile,"")&&strcmp(logFile,"-"))
		// Settato il file di log
		StdLog = fopen(logFile,"a+");
	if (NULL==StdLog)
	{
		StdLog = stdout;
		qDebug("StdLog==NULL");
	}
	// No bufferizzazione
	setvbuf(StdLog, (char *)NULL, _IONBF, 0);
	setvbuf(stdout, (char *)NULL, _IONBF, 0);
	setvbuf(stderr, (char *)NULL, _IONBF, 0);
	// D'ora in avanti qDebug, ... scrivono dove gli ho detto io
	qInstallMsgHandler( myMessageOutput );

	QString language_suffix = getLanguage();
	if (!language_suffix.isNull())
	{
		QString language_file;
		language_file.sprintf(LANGUAGE_FILE_TMPL, language_suffix.ascii());
		QTranslator *translator = new QTranslator(0);
		if (translator->load(language_file))
			a.installTranslator(translator);
		else
			qWarning("File %s not found for language %s", language_file.ascii(), language_suffix.ascii());
	}

	/*******************************************
	 ** Fine Lettura configurazione applicativo
	 *******************************************/

	signal(SIGUSR1, MySignal);
	signal(SIGUSR2, ResetTimer);

	qDebug("Start BtMain");

	BTouch = new BtMain(NULL,"MAIN PROGRAM",&a);

	return a.exec();
}
