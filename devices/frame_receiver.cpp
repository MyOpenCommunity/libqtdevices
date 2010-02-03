#include "frame_receiver.h"
#include "openclient.h" // client_monitor

// Inizialization of static member
QHash<int, Client*> FrameReceiver::clients_monitor;


FrameReceiver::FrameReceiver(int oid)
{
	subscribed = false;
	openserver_id = oid;
}

FrameReceiver::~FrameReceiver()
{
	if (subscribed)
		clients_monitor[openserver_id]->unsubscribe(this);
}

void FrameReceiver::setClientsMonitor(const QHash<int, Client*> &monitors)
{
	clients_monitor = monitors;
}

void FrameReceiver::subscribe_monitor(int who)
{
	Q_ASSERT_X(clients_monitor.contains(openserver_id), "FrameReceiver::subscribe_monitor",
		qPrintable(QString("Client monitor not set for id: %1!").arg(openserver_id)));
	clients_monitor[openserver_id]->subscribe(this, who);
	subscribed = true;
}

