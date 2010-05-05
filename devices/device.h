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
#include <QSignalMapper>

class device_status;
class frame_interpreter;
class Client;
class OpenMsg;

typedef QHash<int, QVariant> DeviceValues;


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
	void initDevices();

private:
	Client *monitor, *command, *request;
	int openserver_id;
	bool is_connected;
	QBasicTimer connection_timer;
};


//! Generic device
class device : public QObject, FrameReceiver
{
friend class TestDevice;
friend class BtMain;
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

	// The following method can be reimplemented in order to parse the incoming
	// frames (from the client monitor). However, if the specific device can be
	// subclassed, reimplement the parseFrame method in order to avoid a double
	// valueReceived signal.
	virtual void manageFrame(OpenMsg &msg);

	bool isConnected();
	int openserverId();

	static void sendCommandFrame(int openserver_id, const QString &frame);

signals:
	// TODO: Old Status changed, to be removed asap.
	void status_changed(QList<device_status*>);

	/// The valueReceived signal, used to inform that a dimension of device
	/// has changed or a new command has received. For some devices, more than
	/// one dimension can change at the same time, so the int is used as an enum
	/// to recognize the dimensions.
	/// Note that using an int is a design choice. In this way the signal is
	/// generic (so the connections can be made in a generic way) and the enum
	/// can be specific for a device, avoiding the coupling between abstract
	/// and concrete device class.
	void valueReceived(const DeviceValues &values_list);

	void connectionUp();
	void connectionDown();

public slots:
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	// queues the frame to be emitted after a time interval; if another compressed
	// frame with the same "what" is sent before the timeout, the first frame is
	// discarded and the timeout restarted
	void sendCompressedFrame(QString frame) const;

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
	void sendCommand(int what) const;
	void sendRequest(QString what) const;
	void sendRequest(int what) const;

	// This should be the preferred way to parse the incoming frames (see the comment
	// above regarding the manageFrame method). Return true if the argument frame
	// was recognized and processed.
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list) { return false; }

private:
	static OpenServerManager *getManager(int openserver_id);

private slots:
	void emitCompressedFrame(int what);

private:
	static QHash<int, QPair<Client*, Client*> > clients;
	static QHash<int, OpenServerManager*> openservers;

	mutable QHash<int, QPair<QTimer*, QString> > compressed_frames;
	mutable QSignalMapper compressor_mapper;
};

#endif //__DEVICE_H__
