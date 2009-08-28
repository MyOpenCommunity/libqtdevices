#ifndef FRAME_RECEIVER_H
#define FRAME_RECEIVER_H

class OpenMsg;


/**
 * This class should be an abstract class, in order to offer a common interface
 * for all the objects that would manage frames from socket (inheriting from this
 * class).
 *
 * A day in the future, manageFrame should be a virtual pure method, the only
 * one in this class.
 */
class FrameReceiver
{
public:
	virtual void manageFrame(OpenMsg &msg);
	virtual void frame_rx_handler(char *frame) {}
};


#endif // FRAME_RECEIVER_H
