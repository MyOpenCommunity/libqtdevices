#include "poweramplifier_device.h"
#include "generic_functions.h" // createRequestOpen


PowerAmplifierDevice::PowerAmplifierDevice(QString address) :
	device(QString("22"), "3#" + address[0] + "#" + address[1])
{
}


void PowerAmplifierDevice::frame_rx_handler(char *frame)
{
	// We cannot use OpenMsg because where is not an int!!
}

void PowerAmplifierDevice::sendRequest(int what) const
{
	sendInit(createRequestOpen(who, QString::number(what), where));
}

void PowerAmplifierDevice::requestStatus() const
{
	sendRequest(12);
}

