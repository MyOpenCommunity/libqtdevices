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
#include "genericfunz.h"
#include <unistd.h>



Client::Client( const QString &host, Q_UINT16 port, int ismon)
{
//   qDebug( "Creating Client");      
   
  ismonitor=ismon;
 //   memset(&fr[0],'\000',sizeof(fr));
 
  socket = new QSocket( this );
 
  connect( socket, SIGNAL(connected()),SLOT(socketConnected()) );
  connect( socket, SIGNAL(connectionClosed()),SLOT(socketConnectionClosed()) );
  connect( socket, SIGNAL(readyRead()),SLOT(socketFrameRead()) );
  connect( socket, SIGNAL(error(int)),SLOT(socketError(int)) );

  tick = NULL;
  
  // connect to the server
  connetti();

  // azzero la variabile last_msg_open_read e last_msg_open_write
  last_msg_open_read.CreateNullMsgOpen();
  last_msg_open_write.CreateNullMsgOpen();
  
/*  if (ismonitor) 
      qDebug( "Monitor");      
  else 
     qDebug( "Command");      
*/      
  /* if (ismonitor) 
   {
       connetti();
   }*/  
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
    emit(monitorSu());
  }
  else {
    qDebug( "TRY TO START command");
    //socket->clearPendingData ();
//    sendToServer(SOCKET_COMANDI ); 

//    memset(&fr[0],'\000',sizeof(fr));
  }
      
}
/****************************************************************************
**
** connetto all'openserver
**
*****************************************************************************/
void Client::ApriInviaFrameChiudi(char* frame)
{
    if(strcmp(frame, last_msg_open_write.frame_open) != 0)
    {
        last_msg_open_write.CreateMsgOpen(frame,strlen(frame));
        if( ( socket->state() == QSocket::Idle )|| ( socket->state() == QSocket::Closing ))
        {
	    //strcpy(&fr[0],frame);
	    connetti();
	    sendToServer(SOCKET_COMANDI ); //lo metto qui else mando prima frame di questo!
        }
        sendToServer(frame);
        qDebug("invio: %s",frame);
    }
    else
       qDebug("Frame Open <%s> already send", frame);
#if 0
    openwebnet msg_open;
    msg_open.CreateMsgOpen(frame,strlen(frame));	  
    if ( (!strcmp(msg_open.Extract_chi(),"1")) || (!strcmp(msg_open.Extract_chi(),"2")) )
    {
	QString s=msg_open.Extract_dove();
	if (s.length()==1) 
	    emit(frameToAutoread(msg_open.frame_open));
    }
#endif    
}

void Client::ApriInviaFrameChiudiw(char *frame)
{
    qDebug("Client::ApriInviaFrameChiudiw()");
    ApriInviaFrameChiudi(frame);
    qDebug("Frame sent waiting for ack");
    while(socketWaitForAck() < 0);
    qDebug("Ack received");
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
  //  qDebug( "Trying to connect to the local openserver" );
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
//  qDebug( "Written %d bytes",socket->writeBlock( frame, strlen(frame) ));
  socket->writeBlock( frame, strlen(frame));
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
  
  //riarmo il WD
  rearmWDT();
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
	  qDebug("letto: %s", buf);
          if(strcmp(buf, last_msg_open_read.frame_open) !=0 )
          {
            last_msg_open_read.CreateMsgOpen(buf,strlen(buf));
            emit frameIn(buf);
          }
          else
            qDebug("Frame Open duplicated");
      } else {
	  if(!strcmp(buf, "*#*1##")) {
	      qDebug("ack received");
              last_msg_open_write.CreateNullMsgOpen();
	      emit openAckRx();
	  }
	  else if(!strcmp(buf, "*#*0##")) {
	      qDebug("nak received");
              last_msg_open_write.CreateNullMsgOpen();
	      emit openNakRx();
	  }
      }
}
return 0;  
}

// Aspetta ack
int Client::socketWaitForAck()
{
    if(ismonitor) return -1;
    ackRx = false;
    connect(this, SIGNAL(openAckRx()), this, SLOT(ackReceived()));
    int stat;
    if((stat = socketFrameRead()) < 0)
	return stat;
    disconnect(this, SIGNAL(openAckRx()), this, SLOT(ackReceived()));
    return ackRx ? 0 : -1 ;
}

void Client::ackReceived()
{
    ackRx = true ;
}

/****************************************************************************
**
** chiusa dal server
**
*****************************************************************************/
void Client::socketConnectionClosed()
{
  qDebug( "Connection closed by the server");    
  if (ismonitor)
      connetti();
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
    if(tick)
      delete tick;
    tick = new QTimer(this,"tick");
    tick->start(500,TRUE);
  //  qDebug( "Monitor: riprovo a connettermi");
    connect(tick,SIGNAL(timeout()), this,SLOT(connetti()));  
  }
}

//
// EOF
//

