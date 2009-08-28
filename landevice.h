/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the specific device for network settings.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef LANDEVICE_H
#define LANDEVICE_H

#include "device.h"

#include <QHash>
#include <QVariant>


class OpenMsg;


/**
 * \class LanDevice
 *
 * This class represent a device for managing lan settings. It has a method to
 * activate/deactivate the lan, and some methods to obtain information about
 * actual settings. When a request of information is done, the response is sent
 * through the signal status_changed.
 */
class LanDevice : public device
{
Q_OBJECT
public:
	LanDevice();
	void enableLan(bool enable);

	// The request methods, used to request an information
	void requestIp() const;
	void requestNetmask() const;
	void requestMacAddress() const;
	void requestGateway() const;
	void requestDNS1() const;
	void requestDNS2() const;

	virtual void manageFrame(OpenMsg &msg);

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
	void requestStatus() const;

private:
	void sendRequest(int what) const;
};

#endif // LANDEVICE_H
