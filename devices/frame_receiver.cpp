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

void FrameReceiver::setClientMonitor(Client *monitor)
{
	clients_monitor[0] = monitor;
}

void FrameReceiver::subscribe_monitor(int who)
{
	clients_monitor[openserver_id]->subscribe(this, who);
	subscribed = true;
}

