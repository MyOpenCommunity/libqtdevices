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

#ifndef OPENSERVER_ADDR
#define OPENSERVER_ADDR "127.0.0.1"
#endif

#define OPENSERVER_PORT 20000


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

	enum Type
	{
		MONITOR = 0,
		RICHIESTE,
		COMANDI
	};

	Client(Type t, const QString &_host=OPENSERVER_ADDR, unsigned _port=OPENSERVER_PORT);
	void ApriInviaFrameChiudi(const char *);

private slots:
	void connetti();
	/// Reads messages from the socket
	int socketFrameRead(void);

	void socketConnected();
	void socketConnectionClosed();
	void socketError(QAbstractSocket::SocketError e);

	/// Send an \a Open \aFrame through the socket and wait for ack
	void ApriInviaFrameChiudiw(char*);
	void richStato(QString richiesta);
	void ackReceived();

	void clear_last_msg_open_read();

private:
	QTcpSocket *socket;
	Type type;
	QString host;
	unsigned port;
	QByteArray data_read;
	QTimer Open_read;
	openwebnet last_msg_open_read;
	openwebnet last_msg_open_write;
	bool ackRx;

	void socketStateRead(char*);
	void manageFrame(QByteArray frame);
	QByteArray readFromServer();

	//! Wait for ack (returns 0 on ack, -1 on nak or when socket is a monitor socket)
	int socketWaitForAck();

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
