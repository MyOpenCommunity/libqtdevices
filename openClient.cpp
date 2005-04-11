/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** openClient.cpp
**
** Finestra principale
**
****************************************************************/

#include <qsocket.h>
#include <qtextstream.h>
#include "openclient.h"
#include <unistd.h>



Client::Client( const QString &host, Q_UINT16 port, int ismon)
{
//   qDebug( "Creating Client");      
   
  ismonitor=ismon;
    
 
  socket = new QSocket( this );
 
  connect( socket, SIGNAL(connected()),SLOT(socketConnected()) );
  connect( socket, SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()) );
  connect( socket, SIGNAL(readyRead()),SLOT(socketFrameRead()) );
  connect( socket, SIGNAL(error(int)),SLOT(socketError(int)) );
  
  // connect to the server
//  connetti();
  
/*  if (ismonitor) 
      qDebug( "Monitor");      
  else 
     qDebug( "Command");      
*/      
   if (ismonitor) 
   {
       connetti();
   }  
}

/****************************************************************************
**
** socket connessa 
**
*****************************************************************************/
void Client::socketConnected()
{
  qDebug( "Connected to server");
  if (ismonitor)  {
    qDebug( "TRY TO START monitor session");
    socket->clearPendingData ();
    sendToServer(SOCKET_MONITOR);
  }
  else {
    qDebug( "TRY TO START command");
    //socket->clearPendingData ();
    sendToServer(SOCKET_COMANDI ); 
  }
      
}
/****************************************************************************
**
** connetto all'openserver
**
*****************************************************************************/
void Client::ApriInviaFrameChiudi(char* frame)
{
        if ( socket->state() == QSocket::Idle )
    {
	connetti();
    }
    sendToServer(frame);
    qDebug("invio: %s",frame);
//    sleep(2);
//    closeConnection();
}

/****************************************************************************
**
** richiesta stato all'openserver
**
*****************************************************************************/
void Client::richStato(char* richiesta)
{   
    if ( socket->state() == QSocket::Idle )
    {
	connetti();
    }
    sendToServer(richiesta);
}

/****************************************************************************
**
** connetto all'openserver
**
*****************************************************************************/
void Client::connetti()
{    
   qDebug( "Trying to connect to the local openserver" );
//   socket->clearPendingData ();
   socket->connectToHost( "127.0.0.1", 20000 );  
}

/****************************************************************************
**
** chiudo
**
*****************************************************************************/
void Client::closeConnection()
{
  socket->close();
  if ( socket->state() == QSocket::Closing ) {
    // We have a delayed close.
    connect( socket, SIGNAL(delayedCloseFinished()),SLOT(socketClosed()) );
  } else {
    // The socket is closed.
    //  socketClosed();
  }
}
	
/****************************************************************************
**
** invio all'openserver
**
*****************************************************************************/
void Client::sendToServer(char * frame)
{
 // qDebug( "Writing");
  qDebug( "Written %d bytes",socket->writeBlock( frame, strlen(frame) ));
//  QTextStream os(socket);
//  os << "\n";
//  os << frame;
//  os << "\n";
//  qDebug( "End");
}

/****************************************************************************
**
** leggo frame
**
*****************************************************************************/
int Client::socketFrameRead()
{
  char buf[2048];
  char * pnt;
  int num=0,n_read=0;
  
  // read from the server
   
  for(;;)
  {
      memset(buf,'\0',sizeof(buf));
      pnt=buf;
      n_read=0;
      while(true){
	  if (pnt<(buf+sizeof(buf)-1)) {	 
	      num=socket->getch();
	       if  ( (num==-1) && (pnt==buf) )
		  break;
	       else if (num!=-1)
	       {
	            *pnt=(char)num;
	            pnt++;
	            n_read++;
	      }
	  }
	  else return -1;
	  if (n_read>=2)
	      if ( (buf[n_read-1] == '#') && (buf[n_read-2] == '#') )
		  break;      
      }
      if (num==-1)
	  break;
      
      // inizializzo il buffer
      buf[n_read]='\0';
      n_read--;
      
      //elimino gli eventuali caratteri '\n' e '\r'
      while((buf[n_read]=='\n') || (buf[n_read]=='\r'))
      {
	  // inizializzo il buffer
	  buf[n_read]='\0';
	  n_read--;
      }  
      
         if  (ismonitor)
      {
	  //qDebug( "letta monitor");
	  openwebnet msg_open;
	  msg_open.CreateMsgOpen(buf,strlen(buf));	  
	  qDebug("letto: %s", msg_open.frame_open);
	  emit frameIn(msg_open.frame_open);
      }
}
  
}



/****************************************************************************
**
** chiusa dal server
**
*****************************************************************************/
void Client::socketConnectionClosed()
{
  qDebug( "Connection closed by the server");    
}

/****************************************************************************
**
** chiusa da me ??
**
*****************************************************************************/
void Client::socketClosed()
{
//  qDebug("Connection closed");    
}

/****************************************************************************
**
** errore
**
*****************************************************************************/
void Client::socketError( int e )
{
  qDebug( "Error number %d occurred",e);
  if (ismonitor) {
    tick = new QTimer(this,"tick");
    tick->start(2500,TRUE);
    qDebug( "Monitor: riprovo a connettermi");
    connect(tick,SIGNAL(timeout()), this,SLOT(connetti()));  
  }
}

//
// EOF
//

