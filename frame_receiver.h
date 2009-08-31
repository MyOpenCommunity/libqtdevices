#ifndef FRAME_RECEIVER_H
#define FRAME_RECEIVER_H

class Client;
class OpenMsg;


/**
 * This class should be an abstract class, in order to offer a common interface
 * for all the objects that would manage frames from socket (inheriting from this
 * class).
 *
 * A day in the future, manageFrame should be a virtual pure method.
 */
class FrameReceiver
{
public:
	FrameReceiver();
	virtual ~FrameReceiver();
	static void setClientMonitor(Client *monitor);

	// The method called every time that a frame with the 'who' subscribed arrive
	// from the openserver.
	virtual void manageFrame(OpenMsg &msg);

	// TODO: For compatibility with the old structure, remove it as soon as possibile.
	virtual void frame_rx_handler(char *frame) {}

protected:
	// Every child that would receive a frame for a 'who' must subscribe itself
	// calling this method (unregistration is not needed).
	void subscribe_monitor(int who);

private:
	static Client *client_monitor;
	bool subscribed;
};


#endif // FRAME_RECEIVER_H
