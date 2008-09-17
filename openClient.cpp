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

#include <qsocket.h>
#include <qtextstream.h>
#include <unistd.h>

Client::Client(const QString &host, Q_UINT16 port, int ismon, bool isri)
{
	qDebug("Client::Client()");

	ismonitor=ismon;
	isrichiesta=isri;

	socket = new QSocket(this);

	connect(socket, SIGNAL(connected()),this, SLOT(socketConnected()));
	connect(socket, SIGNAL(connectionClosed()),this, SLOT(socketConnectionClosed()));
	connect(socket, SIGNAL(readyRead()),this, SLOT(socketFrameRead()));
	connect(socket, SIGNAL(error(int)),this, SLOT(socketError(int)));

	tick = NULL;

	// connect to the server
	connetti();

	// azzero la variabile last_msg_open_read e last_msg_open_write
	last_msg_open_read.CreateNullMsgOpen();
	last_msg_open_write.CreateNullMsgOpen();

	connect(&Open_read, SIGNAL(timeout()), this, SLOT(clear_last_msg_open_read()));
}

/****************************************************************************
**
** socket connessa 
**
*****************************************************************************/
void Client::socketConnected()
{
	qDebug("Client::socketConnected()");
	qDebug("Connected to server");
	if (ismonitor)
	{
		qDebug("TRY TO START monitor session");
		socket->clearPendingData ();
		sendToServer(SOCKET_MONITOR);
		emit(monitorSu());
	}
	else if (isrichiesta)
	{
		qDebug("TRY TO START request");
	}
	else
	{
		qDebug("TRY TO START command");
	}
}

// FIX: remove this slot!
void Client::ApriInviaFrameChiudi(char* frame)
{
	ApriInviaFrameChiudi(const_cast<const char*>(frame));
}

/****************************************************************************
**
** connetto all'openserver
**
*****************************************************************************/
void Client::ApriInviaFrameChiudi(const char* frame)
{
	qDebug("Client::ApriInviaFrameChiudi()");

	if (strcmp(frame, last_msg_open_write.frame_open) != 0)
	{
		last_msg_open_write.CreateMsgOpen(const_cast<char*>(frame), strlen(frame));
		if ((socket->state() == QSocket::Idle)|| (socket->state() == QSocket::Closing))
		{
			connetti();
			if (isrichiesta)
				sendToServer(SOCKET_RICHIESTE);
			else
				sendToServer(SOCKET_COMANDI); //lo metto qui else mando prima frame di questo!
		}
		sendToServer(frame);
		qDebug("invio: %s",frame);
	}
	else
		qDebug("Frame Open <%s> already send", frame);
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
	qDebug("Client::richStato()");
	if (socket->state() == QSocket::Idle)
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
	qDebug("Client::connetti()");
	socket->connectToHost("127.0.0.1", 20000);
}

/****************************************************************************
**
** chiudo
**
*****************************************************************************/
void Client::closeConnection()
{
	qDebug("Client::closeConnection()");
	socket->close();
	if (socket->state() == QSocket::Closing)
	{
		// We have a delayed close.
		connect(socket, SIGNAL(delayedCloseFinished()),SLOT(socketClosed()));
	}
}
	
/****************************************************************************
**
** invio all'openserver
**
*****************************************************************************/
void Client::sendToServer(const char * frame)
{
	qDebug("Client::sendToServer()");
	socket->writeBlock(frame, strlen(frame));
}

/****************************************************************************
**
** leggo frame
**
*****************************************************************************/
int Client::socketFrameRead()
{
	char buf[2048];
	char *pnt;
	int num=0, n_read=0;

	qDebug("Client::socketFrameRead()");
	//riarmo il WD
	rearmWDT();
	// read from the server
	for (;;)
	{
		memset(buf,'\0',sizeof(buf));
		pnt=buf;
		n_read=0;
		while(true)
		{
			if (pnt<(buf+sizeof(buf)-1))
			{
				num=socket->getch();
				if  ((num==-1) && (pnt==buf))
					break;
				else if (num!=-1)
				{
					*pnt=(char)num;
					pnt++;
					n_read++;
				}
			}
			else
				return -1;

			if (n_read>=2)
				if ((buf[n_read-1] == '#') && (buf[n_read-2] == '#'))
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

		if (ismonitor)
		{
			qDebug("letto: %s", buf);
			if (strcmp(buf, last_msg_open_read.frame_open) !=0)
			{
				Open_read.stop();
				last_msg_open_read.CreateMsgOpen(buf,strlen(buf));
				Open_read.start(1000, TRUE);
				emit frameIn(buf);
			}
			else
				qDebug("Frame Open duplicated");
		}
		else
		{
			if (!strcmp(buf, "*#*1##"))
			{
				qDebug("ack received");
				last_msg_open_write.CreateNullMsgOpen();
				emit openAckRx();
			}
			else if (!strcmp(buf, "*#*0##"))
			{
				qDebug("nak received");
				last_msg_open_write.CreateNullMsgOpen();
				emit openNakRx();
			}
		}
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

	if (ismonitor)
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

/****************************************************************************
**
** chiusa dal server
**
*****************************************************************************/
void Client::socketConnectionClosed()
{
	qDebug("Client::socketConnectionClosed()");
	qDebug("Connection closed by the server");
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
	qDebug("Client::socketClosed()");
}

/****************************************************************************
**
** errore
**
*****************************************************************************/
void Client::socketError(int e)
{
	qDebug("Client::socketError()");

	qDebug("Error number %d occurred",e);
	if (ismonitor)
	{
		if (tick)
			delete tick;
		tick = new QTimer(this,"tick");
		tick->start(500,TRUE);
		connect(tick,SIGNAL(timeout()), this,SLOT(connetti()));
	}
}
