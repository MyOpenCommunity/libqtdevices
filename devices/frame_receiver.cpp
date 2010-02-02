#include "frame_receiver.h"
#include "openclient.h" // client_monitor

// Inizialization of static member
QHash<int, Client*> FrameReceiver::clients_monitor;


FrameReceiver::FrameReceiver()
{
	subscribed = false;
}

FrameReceiver::~FrameReceiver()
{
	if (subscribed)
		clients_monitor[0]->unsubscribe(this);
}

void FrameReceiver::setClientMonitor(Client *monitor)
{
	clients_monitor[0] = monitor;
}

void FrameReceiver::subscribe_monitor(int who)
{
	clients_monitor[0]->subscribe(this, who);
	subscribed = true;
}

