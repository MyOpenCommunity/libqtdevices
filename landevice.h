#ifndef LANDEVICE_H
#define LANDEVICE_H

#include "device.h"

#include <QHash>
#include <QVariant>


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

	enum Type
	{
		DIM_STATUS = 9,
		DIM_IP = 10,
		DIM_NETMASK = 11,
		DIM_MACADDR = 12,
		DIM_GATEWAY = 50,
		DIM_DNS1 = 51,
		DIM_DNS2 = 52
	};

public slots:
	virtual void frame_rx_handler(char *);

private:
	void sendRequest(int what);

signals:
	void status_changed(QHash<int, QVariant> status_list);
};

#endif // LANDEVICE_H
