/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the specific device for energy management.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef ENERGY_DEVICE_H
#define ENERGY_DEVICE_H

#include "device.h"

#include <QHash>
#include <QVariant>

class EnergyDevice : public device
{
Q_OBJECT
public:
	EnergyDevice(QString where);

	// The request methods, used to request an information
	void requestCumulativeDay() const;

public slots:
	void frame_rx_handler(char *frame);

private:
	void sendRequest(int what) const;

#if 0
	void enableLan(bool enable);

	// The request methods, used to request an information
	void requestStatus() const;
	void requestIp() const;
	void requestNetmask() const;
	void requestMacAddress() const;
	void requestGateway() const;
	void requestDNS1() const;
	void requestDNS2() const;

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
	void sendRequest(int what) const;
#endif
};
#endif // ENERGY_DEVICE_H
