#include "frame_receiver.h"

#include <openmsg.h>


void FrameReceiver::manageFrame(OpenMsg &msg)
{
	frame_rx_handler(msg.frame_open);
}

