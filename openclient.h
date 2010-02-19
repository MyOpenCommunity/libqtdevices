#ifndef OPENCLIENT_H
#define OPENCLIENT_H

#include <openwebnet.h>

#include <QTcpSocket>
#include <QByteArray>
#include <QHash>

#ifndef OPENSERVER_ADDR
#define OPENSERVER_ADDR "127.0.0.1"
#endif

#define OPENSERVER_PORT 20000

// The id for the main openserver
#define MAIN_OPENSERVER 0

class FrameReceiver;


/**
 * This class manages the socket communication throught the application and the openserver.
 *
 * Clients of type MONITOR can be used to receive frames from openserver while clients
 * of types COMMANDI or RICHIESTE can be used to send frames to the openserver.
 */
class Client : public QObject
{
friend class OpenServerMock;
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:

	enum Type
	{
		MONITOR = 0,
		SUPERVISOR,
		RICHIESTE,
		COMANDI
	};

	Client(Type t, const QString &_host=OPENSERVER_ADDR, unsigned _port=0);
	void sendFrameOpen(const QString &frame_open);

	void subscribe(FrameReceiver *obj, int who);
	void unsubscribe(FrameReceiver *obj);

	bool isConnected();

#if DEBUG
	void flush() { socket->flush(); }
	void forwardFrame(Client *c);
#endif

signals:
	// The signals connectionUp/connectionDown are emitted only for clients
	// of the monitor type.
	void connectionUp();
	void connectionDown();

	//! Openwebnet ack received
	void openAckRx();
	//! Openwebnet nak received
	void openNakRx();

private slots:
	void connectToHost();
	/// Reads messages from the socket
	int socketFrameRead();

	void socketConnected();
	void socketConnectionClosed();
	void socketError(QAbstractSocket::SocketError e);

	void ackReceived();

private:
	QTcpSocket *socket;
	Type type;
	// The address of the openserver
	QString host;
	// The port of the openserver
	unsigned port;

	// The buffer that store the data read from the server
	QByteArray data_read;

	bool ackRx;

	// The list of the FrameReceivers that will receive the incoming frames.
	QHash<int, QList<FrameReceiver*> > subscribe_list;

	// We use this flag instead of the state() method of the QTcpSocket in order
	// to avoid the emission of the signal disconnected() when the socket is
	// already disconnected.
	bool is_connected;

#if DEBUG
	Client *to_forward;
#endif

	void manageFrame(QByteArray frame);
	QByteArray readFromServer();

	//! Wait for ack (returns 0 on ack, -1 on nak or when socket is a monitor socket)
	int socketWaitForAck();
	void dispatchFrame(QString frame);
};

#endif
