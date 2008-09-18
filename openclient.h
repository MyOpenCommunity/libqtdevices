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

#include <openwebnet.h>

#include <QTimer>
#include <QTcpSocket>
#include <QByteArray>

#define SOCKET_MONITOR "*99*1##"
#define SOCKET_COMANDI "*99*9##"
#define SOCKET_RICHIESTE "*99*0##"


/*!
  \class Client
  \brief This class manages the socket communication throught the application and the \a openserver.

  \author Davide
  \date lug 2005
*/

class Client : public QObject
{
Q_OBJECT
public:
	Client(const QString &host, unsigned _port, int mon, bool richieste=false);
	~Client() {};

public slots:
	/// Connects to the socket
	void connetti();
	void ApriInviaFrameChiudi(const char *);
	void ApriInviaFrameChiudi(char *);

private slots:
 	/// Closes the socket
	void closeConnection(void);
	
	/// Reads messages from the socket
	int socketFrameRead(void);
	//! Wait for ack (returns 0 on ack, -1 on nak or when socket is a monitor socket)
	int socketWaitForAck(void);

	void socketConnected(void);
	void socketConnectionClosed(void);
	void socketClosed(void);
	void socketError(QAbstractSocket::SocketError e);

	/// Send an \a Open \aFrame through the socket and wait for ack
	void ApriInviaFrameChiudiw(char*);
	void richStato(char*);
	void ackReceived(void);

	void clear_last_msg_open_read();

private:
	QTcpSocket *socket;
	int ismonitor;
	bool isrichiesta;
	unsigned port;
	QByteArray data_read;
	QTimer Open_read;
	openwebnet last_msg_open_read;
	openwebnet last_msg_open_write;

	/// Sends messages throught the socket
	void sendToServer(const char *);
	void socketStateRead(char*);
	bool ackRx;
	void manageFrame(QByteArray frame);
	QByteArray readFromServer();

signals:
	void frameIn(char*);
	void rispStato(char*);
	void monitorSu();
	void frameToAutoread(char*);
	//! Openwebnet ack received
	void openAckRx();
	//! Openwebnet nak received
	void openNakRx();
};

#endif
