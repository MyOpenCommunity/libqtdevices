#include "device.h"
#include "openclient.h"
#include "bttime.h"
#include "generic_functions.h" // createRequestOpen, createMsgOpen

#include <openmsg.h>

#include <QDebug>


// Inizialization of static member
QHash<int, QPair<Client*, Client*> > device::clients;


// Device implementation
device::device(QString _who, QString _where, int oid) : FrameReceiver(oid)
{
	who = _who;
	where = _where;
	openserver_id = oid;
	subscribe_monitor(who.toInt());
}

void device::subscribe_monitor(int who)
{
	FrameReceiver::subscribe_monitor(who);
	connect(clients_monitor[openserver_id], SIGNAL(connectionUp()), SIGNAL(connectionUp()));
	connect(clients_monitor[openserver_id], SIGNAL(connectionDown()), SIGNAL(connectionDown()));
}

bool device::isConnected()
{
	return clients_monitor[openserver_id]->isConnected();
}

void device::sendFrame(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].first, "device::sendFrame",
			   qPrintable(QString("Client comandi not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].first->sendFrameOpen(frame.toAscii().constData());
}

void device::sendInit(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].second, "device::sendInit",
		qPrintable(QString("Client richieste not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].second->sendFrameOpen(frame.toAscii().constData());
}

void device::sendCommand(QString what, QString _where) const
{
	sendFrame(createMsgOpen(who, what, _where));
}

void device::sendCommand(QString what) const
{
	sendCommand(what, where);
}

void device::sendRequest(QString what) const
{
	if (what.isEmpty())
		sendInit(createStatusRequestOpen(who, where));
	else
		sendInit(createRequestOpen(who, what, where));
}

void device::setClients(const QHash<int, QPair<Client*, Client*> > &c)
{
	clients = c;
}

QString device::get_key()
{
	return QString("%1#%2*%3").arg(openserver_id).arg(who).arg(where);
}

