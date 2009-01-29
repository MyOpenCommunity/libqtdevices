#ifndef LANDEVICE_H
#define LANDEVICE_H

#include "device.h"

class LanDevice : public device
{
Q_OBJECT
public:
	LanDevice();
	void enableLan(bool enable);

	// The request methods, used to request an information
	void requestStatus();
	void requestIp();
	void requestNetmask();
	void requestMacAddress();
	void requestGateway();
	void requestDNS1();
	void requestDNS2();

public slots:
	virtual void frame_rx_handler(char *);

private:
	void sendRequest(int what);

};

#endif // LANDEVICE_H
