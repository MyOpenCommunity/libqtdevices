/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** openClient.cpp
**
** Finestra principale
**
****************************************************************/

#include "openclient.h"
#include "genericfunz.h"

#define SOCKET_MONITOR "*99*1##"
#define SOCKET_COMANDI "*99*9##"
#define SOCKET_RICHIESTE "*99*0##"


Client::Client(Type t, const QString &_host, unsigned _port) : type(t), host(_host), port(_port)
{
	qDebug("Client::Client()");

	socket = new QTcpSocket(this);

	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(disconnected()), SLOT(socketConnectionClosed()));
	connect(socket, SIGNAL(readyRead()), SLOT(socketFrameRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));

	// connect to the server
	connetti();

	// azzero la variabile last_msg_open_read e last_msg_open_write
	last_msg_open_read.CreateNullMsgOpen();
	last_msg_open_write.CreateNullMsgOpen();

	connect(&Open_read, SIGNAL(timeout()), this, SLOT(clear_last_msg_open_read()));
}

void Client::socketConnected()
{
	qDebug("Client::socketConnected()");
	qDebug("Connected to server");
	if (type == MONITOR)
	{
		qDebug("TRY TO START monitor session");
		socket->write(SOCKET_MONITOR);
		emit monitorSu();
	}
	else if (type == RICHIESTE)
		qDebug("TRY TO START request");
	else
		qDebug("TRY TO START command");
}

// FIX: remove this slot!
void Client::ApriInviaFrameChiudi(char* frame)
{
	ApriInviaFrameChiudi(const_cast<const char*>(frame));
}

void Client::ApriInviaFrameChiudi(const char* frame)
{
	qDebug("Client::ApriInviaFrameChiudi()");

	if (strcmp(frame, last_msg_open_write.frame_open) != 0)
	{
		last_msg_open_write.CreateMsgOpen(const_cast<char*>(frame), strlen(frame));
		if (socket->state() == QAbstractSocket::UnconnectedState || socket->state() == QAbstractSocket::ClosingState)
		{
			connetti();
			if (type == RICHIESTE)
				socket->write(SOCKET_RICHIESTE);
			else
				socket->write(SOCKET_COMANDI); //lo metto qui else mando prima frame di questo!
		}
		socket->write(frame);
		qDebug("invio: %s",frame);
	}
	else
		qDebug("Frame Open <%s> already send", frame);

	// TODO: questa funzione dovra' gestire anche i Nak e ack (e la sua versione "w"
	// dovra' sparire), attendendo che arrivi o un ack o un nack con un ciclo tipo:
	// while (socketWaitForAck() < 0 || socketWaitForNak() < 0);
	// restituendo quindi un booleano che vale true se e' un ack, false altrimenti.
}

void Client::ApriInviaFrameChiudiw(char *frame)
{
	qDebug("Client::ApriInviaFrameChiudiw()");
	ApriInviaFrameChiudi(frame);
	qDebug("Frame sent waiting for ack");
	while (socketWaitForAck() < 0);
	qDebug("Ack received");
}

// richiesta stato all'openserver
void Client::richStato(char* richiesta)
{
	qDebug("Client::richStato()");
	if (socket->state() == QAbstractSocket::UnconnectedState)
		connetti();
	socket->write(richiesta);
}

void Client::connetti()
{
	qDebug("Client::connetti()");
	socket->connectToHost(host, port);
}

QByteArray Client::readFromServer()
{
	data_read += socket->readAll();
	int pos = data_read.indexOf("##");
	if (pos == -1)
		return QByteArray();
	else
	{
		QByteArray buf = data_read.left(pos + 2);
		data_read.remove(0, pos + 2);
		return buf;
	}
}

void Client::manageFrame(QByteArray frame)
{
	if (type == MONITOR)
	{
		qDebug() << "frame read: " << frame;
		if (frame == "*#*1##")
			qWarning("ERROR - ack received");
		else if (frame == "*#*0##")
			qWarning("ERROR - nak received");
		else if (frame != last_msg_open_read.frame_open)
		{
			Open_read.stop();
			last_msg_open_read.CreateMsgOpen(frame.data(),frame.size());
			Open_read.setSingleShot(true);
			Open_read.start(1000);
			emit frameIn(frame.data());
		}
		else
			qDebug("Frame Open duplicated");
	}
	else
	{
		if (frame == "*#*1##")
		{
			qDebug("ack received");
			last_msg_open_write.CreateNullMsgOpen();
			emit openAckRx();
		}
		else if (frame == "*#*0##")
		{
			qDebug("nak received");
			last_msg_open_write.CreateNullMsgOpen();
			emit openNakRx();
		}
	}
}

int Client::socketFrameRead()
{
	qDebug("Client::socketFrameRead()");
	//riarmo il WD
	rearmWDT();

	while (true)
	{
		QByteArray frame = readFromServer();

		while (frame.endsWith('\n') || frame.endsWith('\r'))
			frame.chop(1);

		if (frame.isEmpty())
			break;

		manageFrame(frame);
	}
	return 0;
}

void Client::clear_last_msg_open_read()
{
	qDebug("Delete last Frame Open read");
	last_msg_open_read.CreateNullMsgOpen();
}

// Aspetta ack
int Client::socketWaitForAck()
{
	qDebug("Client::socketWaitForAck()");

	if (type == MONITOR)
		return -1;
	ackRx = false;
	connect(this, SIGNAL(openAckRx()), this, SLOT(ackReceived()));
	int stat;
	if ((stat = socketFrameRead()) < 0)
		return stat;
	disconnect(this, SIGNAL(openAckRx()), this, SLOT(ackReceived()));
	return ackRx ? 0 : -1;
}

void Client::ackReceived()
{
	ackRx = true;
}

void Client::socketConnectionClosed()
{
	qDebug("Client::socketConnectionClosed()");
	if (type == MONITOR)
		connetti();
}

void Client::socketError(QAbstractSocket::SocketError e)
{
	if (e != QAbstractSocket::RemoteHostClosedError || type == MONITOR)
		qWarning() << "OpenClient: error " << e << "occurred " << socket->errorString()
			<< "on client" << type;

	if (type == MONITOR)
		QTimer::singleShot(500, this, SLOT(connetti()));
}
