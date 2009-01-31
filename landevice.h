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


/**
 * \class LanDevice
 *
 * This class represent a device for managing lan settings. It has a method to
 * activate/deactivate the lan, and some methods to obtain informations about
 * actual settings. When a request of informations is done, the response is send
 * through the signal status_changed.
 */
class LanDevice : public device
{
Q_OBJECT
public:
	LanDevice();
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

signals:
	// TODO: spostare questo segnale status_changed nella classe device!
	// L'argomento deve rimanere un generico intero piuttosto che un enum in
	// modo da lasciare l'interpretazione del valore alla reale classe device
	// istanziata (visto che l'oggetto grafico conosce la specializzazione del
	// device utilizzata e di conseguenza puo' usare un suo enum per interpretare
	// i valori interi chiave dell'hash, evitando in questo modo dipendenze).

	// The key of hash represent a value in the enum 'Type'
	void status_changed(QHash<int, QVariant> status_list);
};

#endif // LANDEVICE_H
