#ifndef FRAME_RECEIVER_H
#define FRAME_RECEIVER_H

#include <QHash>

class Client;
class OpenMsg;


/**
 * This class is an abstract class, in order to offer a common interface
 * for all the objects that would manage frames from socket (inheriting from this
 * class).
 */
class FrameReceiver
{
public:
	FrameReceiver(int oid=0);
	virtual ~FrameReceiver();
	static void setClientMonitor(Client *monitor);

	// The method called every time that a frame with the 'who' subscribed arrive
	// from the openserver.
	virtual void manageFrame(OpenMsg &msg) = 0;

protected:
	// Every child that would receive a frame for a 'who' must subscribe itself
	// calling this method (unregistration is not needed).
	void subscribe_monitor(int who);

private:
	static QHash<int, Client*> clients_monitor;
	bool subscribed;
	int openserver_id;
};


#endif // FRAME_RECEIVER_H
