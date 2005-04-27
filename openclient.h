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
#define SOCKET_COMANDI "*99*0##"
//#define SOCKET_COMANDI "*99*##"


class Client  : public QSocket
{
    Q_OBJECT
    public:
    Client( const QString &host, Q_UINT16 port, int mon);
    ~Client(){};

//    void ApriInviaFrameChiudi(char *);

    private slots:
    void closeConnection(void);
    void sendToServer(char *);    
    int socketFrameRead(void);
    void socketConnected(void);
    void socketConnectionClosed(void);
    void socketClosed(void);
    void socketError(int e );
    void connetti();
    void ApriInviaFrameChiudi(char *);
    void richStato(char*);
   
    private:
    QSocket *socket;
    int ismonitor;
    QTimer* tick;  
    void socketStateRead(char*);
	    
     signals:
      void  frameIn(char*);
      void rispStato(char*);
    
};

//
// EOF
//


#endif
