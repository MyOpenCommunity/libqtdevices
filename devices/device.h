/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "frame_receiver.h"

#include <QVariant>
#include <QHash>
#include <QList>
#include <QTimer>

class device_status;
class frame_interpreter;
class Client;
class OpenMsg;

typedef QHash<int, QVariant> StatusList;


/**
 * This class manage the logic of disconnection/reconnection with the openserver.
 * Basically, with an openserver you have 3 socket connections open: the monitor,
 * the command and the request. When one of these fall down all the objects
 * connected with that openserver must to be notify with the signal connectionDown.
 * In this case we have to retry the connection after "reconnection_time" seconds.
 * Also in the case of a successfully reconnection the application must to be
 * notified about the event.
 */
class OpenServerManager : public QObject
{
Q_OBJECT
public:
	OpenServerManager(int oid, Client *monitor, Client *command, Client *request);
	bool isConnected();

	// The interval (in seconds) to retry the connection with the openserver.
	static int reconnection_time;

signals:
	void connectionUp();
	void connectionDown();

protected:
	void timerEvent(QTimerEvent*);

private slots:
	void handleConnectionUp();
	void handleConnectionDown();

private:
	Client *monitor, *command, *request;
	int openserver_id;
	bool is_connected;
	int reconnection_timer_id;
};


//! Generic device
class device : public QObject, FrameReceiver
{
friend class TestDevice;
Q_OBJECT

public:
	// Init device: send messages to initialize data. Every device should
	// re-implement this method, in order to update the related graphic object
	// with the right value.
	virtual void init() { }

	//! Returns cache key
	virtual QString get_key();
	virtual ~device() {}

	static void setClients(const QHash<int, QPair<Client*, Client*> > &c);

	virtual void manageFrame(OpenMsg &msg) {}

	bool isConnected();
	int openserverId();

signals:
	// TODO: Old Status changed, to be removed asap.
	void status_changed(QList<device_status*>);

	/// The status changed signal, used to inform that a dimension of device
	/// has changed. For some devices, more than one dimension can change
	/// at the same time, so the int is used as an enum to recognize the dimensions.
	/// Note that using an int is a design choice. In this way the signal is
	/// generic (so the connections can be made in a generic way) and the enum
	/// can be specific for a device, avoiding the coupling between abstract
	/// and concrete device class.
	void status_changed(const StatusList &status_list);

	void connectionUp();
	void connectionDown();

public slots:
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

protected:
	// The costructor is protected only to make device abstract.
	// NOTE: the default openserver id should be keep in sync with the define MAIN_OPENSERVER
	device(QString who, QString where, int openserver_id = 0);

	//! The system of the device
	QString who;
	//! The address of the device
	QString where;

	int openserver_id;

	void sendCommand(QString what, QString _where) const;
	void sendCommand(QString what) const;
	void sendRequest(QString what) const;

private:
	static QHash<int, QPair<Client*, Client*> > clients;
	static QHash<int, OpenServerManager*> openservers;
};

#endif //__DEVICE_H__
