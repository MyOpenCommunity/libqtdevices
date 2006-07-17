/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** openclient.h
**
** Finestra principale
**
****************************************************************/

#ifndef  OPENCLIENT_H
#define OPENCLIENT_H


#include <qsocket.h>
#include <qapplication.h>
#include <qtextstream.h>
#include <qtimer.h>
#include "../bt_stackopen/common_files/openwebnet.h"

#define SOCKET_MONITOR "*99*1##"
#define SOCKET_COMANDI "*99*9##"
//#define SOCKET_COMANDI "*99*0##"
//#define SOCKET_MONITOR "*99*1##"


/*!
  \class Client
  \brief This class manages the socket communication throught the application and the \a openserver.

  \author Davide
  \date lug 2005
*/  

class Client  : public QSocket
{
    Q_OBJECT
    public:
    Client( const QString &host, Q_UINT16 port, int mon);
    ~Client(){};

//    void ApriInviaFrameChiudi(char *);
    public slots:
/*! Connects to the socket
*/  
       void connetti();
    private slots:
/*! Closes the socket
*/          
    void closeConnection(void);
/*! Sends messages throught the socket
*/      
    void sendToServer(char *);    
/*! Reads messages from the socket
*/      
    int socketFrameRead(void);
    //! Wait for ack (returns 0 on ack, -1 on nak or when socket is a monitor socket)
    int socketWaitForAck(void);

    void socketConnected(void);
    void socketConnectionClosed(void);
    void socketClosed(void);
    void socketError(int e );
/*! Send an \a Open \aFrame throught the socket
*/   
    void ApriInviaFrameChiudi(char *);
    /*! Send an \a Open \aFrame through the socket and wait for ack */
    void ApriInviaFrameChiudiw(char*);
    void richStato(char*);
    void ackReceived(void);

    private:
    QSocket *socket;
    int ismonitor;
    QTimer* tick;  
    void socketStateRead(char*);
    bool ackRx;
   // char fr[100];
	    
     signals:
      void  frameIn(char*);
      void rispStato(char*);
      void monitorSu();
      void frameToAutoread(char*);
      //! Openwebnet ack received
      void openAckRx();
      //! Openwebnet nak received
      void openNakRx();
};

//
// EOF
//


#endif
