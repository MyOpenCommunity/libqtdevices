/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** main.cpp
**
**Gestione apertura applicazione
**
****************************************************************/

#include	"btmain.h"
#include <qapplication.h>
#include "../bt_stackopen/common_files/openwebnet.h"
#include "../bt_stackopen/common_files/common_functions.h"
#include "xmlvarihandler.h"
#include <signal.h>
#define	TIMESTAMP
#ifdef TIMESTAMP
#include <qdatetime.h>
#endif

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
int  VERBOSITY_LEVEL=VERBOSITY_LEVEL_DEFAULT;   // condiziona tutte le printf
char * Path_Var=NULL;
char * bt_wd=NULL;
char * My_Path=NULL;
char * Suffisso = "<BTo>";	//Per distinguere le stampe

void myMessageOutput( QtMsgType type, const char *msg )
{
  switch ( type ) {
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
      abort();                    // deliberately core dump
  }
}


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
    char logFile[100];
    
  xmlcfghandler *handler = new xmlcfghandler(&VERBOSITY_LEVEL, &logFile[0]);
  xmlFile = new QFile(My_File_Cfg);
  QXmlInputSource source( xmlFile );
  QXmlSimpleReader reader;
  reader.setContentHandler( handler );
  reader.parse( source );
  delete handler;
  delete xmlFile;
  //-----
    
    
 //   qDebug("<BTo> logfile=%s",My_File_Log);
 //   qDebug("<BTo> logverbosity=%d",VERBOSITY_LEVEL);
    if (strcmp(My_File_Log,"")&&strcmp(My_File_Log,"-"))
      // Settato il file di log
      StdLog = fopen(My_File_Log,"a+");
    if (NULL==StdLog)
      StdLog = stdout;
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
    signal(SIGUSR2, MySignal);    
          
    qDebug("Start BtMain");
    
    BtMain mainprogr(NULL,"MAIN PROGRAM",&a);

    return a.exec();
}






