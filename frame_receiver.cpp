#include "frame_receiver.h"
#include "openclient.h" // client_monitor

#include <openmsg.h>

// Inizialization of static member
Client *FrameReceiver::client_monitor = 0;

FrameReceiver::FrameReceiver()
{
	subscribed = false;
}

FrameReceiver::~FrameReceiver()
{
	if (subscribed)
		client_monitor->unsubscribe(this);
}

void FrameReceiver::manageFrame(OpenMsg &msg)
{
	frame_rx_handler(msg.frame_open);
}

void FrameReceiver::setClientMonitor(Client *monitor)
{
	client_monitor = monitor;
}

void FrameReceiver::subscribe_monitor(int who)
{
	client_monitor->subscribe(this, who);
	subscribed = true;
}

