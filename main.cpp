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
#include "../bt_stackopen/common_files/openwebnet.h"
#include "../bt_stackopen/common_files/common_functions.h"
#include "xmlvarihandler.h"
#include "main.h"

#define	TIMESTAMP
#ifdef TIMESTAMP
#include <qdatetime.h>
#endif

#include <qapplication.h>
#include <signal.h>

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


BtMain *BTouch;

int main( int argc, char **argv )
{
	/*******************************************
	 ** Inizio Lettura configurazione applicativo
	 *******************************************/
	//    qDebug("<BTo> BTouch release");
	//    Leggi_logfile_name(My_File_Cfg,MYPROCESSNAME,&My_File_Log,Xml_File_In,Xml_File_Out);
	//    Leggi_logverbosity(My_File_Cfg,MYPROCESSNAME,&VERBOSITY_LEVEL,Xml_File_In,Xml_File_Out);
	//---

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
	
	xmlcfghandler *handler = new xmlcfghandler(&VERBOSITY_LEVEL, &logFile);
	xmlFile = new QFile(My_File_Cfg);
	QXmlInputSource source( xmlFile );
	QXmlSimpleReader reader;
	reader.setContentHandler( handler );
	reader.parse( source );
	delete handler;
	delete xmlFile;
	//-----
	/*   qDebug("<BTo> logfile=%s",My_File_Log);
	     qDebug("<BTo> logfile=%s",logFile);
	     qDebug("<BTo> logverbosity=%d",VERBOSITY_LEVEL);*/

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

	/*******************************************
	 ** Fine Lettura configurazione applicativo
	 *******************************************/
	QApplication a( argc, argv );

	signal(SIGUSR1, MySignal);
	signal(SIGUSR2, ResetTimer);

	qDebug("Start BtMain");

	BTouch = new BtMain(NULL,"MAIN PROGRAM",&a);

	return a.exec();
}
