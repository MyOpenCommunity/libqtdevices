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


struct Clients
{
	Client *command;
	Client *request;
	Client *supervisor;
	Clients() { command = request = supervisor = 0; }
};

class OpenServerManager : public QObject
{
Q_OBJECT
public:
	OpenServerManager(int oid, Client *monitor, Client *supervisor, Client *command, Client *request);
	bool isConnected();

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
	Client *monitor, *supervisor, *command, *request;
	int openserver_id;
	bool is_connected;
	QBasicTimer connection_timer;
};


// send frames with a delay, removing frames with a duplicate what
class FrameCompressor : public QObject
{
friend class TestDevice;
Q_OBJECT
public:
	FrameCompressor();

	// queues the frame to be emitted after a time interval; if another compressed
	// frame with the same "what" is sent before the timeout, the first frame is
	// discarded and the timer restarted with the new timeout value; when the timer
	// expires, the sendFrame() signal is emitted
	void sendCompressedFrame(QString frame, int compression_timeout) const;

signals:
	// emitted when it is time to send the frame
	void sendFrame(QString frame);

private slots:
	void emitCompressedFrame(int what);

private:
	// used by the tests
	void flushCompressedFrames();

private:
	mutable QHash<int, QPair<QTimer*, QString> > compressed_frames;
	mutable QSignalMapper compressor_mapper;
};


//! Generic device
class device : public QObject, FrameReceiver
{
friend class TestDevice;
friend class BtMain;
Q_OBJECT

public:
	// default timeout value for compressed frames
	static const int COMPRESSION_TIMEOUT = 1000;

	// Init device: send messages to initialize data. Every device should
	// re-implement this method, in order to update the related graphic object
	// with the right value.
	virtual void init() { }

	//! Returns cache key
	virtual QString get_key();
	virtual ~device() {}

	static void setClients(const QHash<int, Clients> &c);

	// The following method can be reimplemented in order to parse the incoming
	// frames (from the client monitor). However, if the specific device can be
	// subclassed, reimplement the parseFrame method in order to avoid a double
	// valueReceived signal.
	virtual void manageFrame(OpenMsg &msg);

	bool isConnected();
	int openserverId();
	static void initDevices();

	// Send (without delay) a frame to the openserver with the given id
	static void sendCommandFrame(int openserver_id, const QString &frame);

	// Set this flag to delay the frames sent using the sendFrame/sendInit methods.
	static void delayFrames(bool delay);

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
	// The following methods send frames to the openserver, using the COMMAND or the REQUEST
	// channel. The frame can be really sent after a delay.
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	// As the previous method, without delay.
	void sendFrameNow(QString frame) const;
	void sendInitNow(QString frame) const;

	// queues the frame to be emitted after a time interval; if another compressed
	// frame with the same "what" is sent before the timeout, the first frame is
	// discarded and the timer restarted with the new timeout value
	void sendCompressedFrame(QString frame, int compression_timeout = COMPRESSION_TIMEOUT) const;
	void sendCompressedInit(QString frame, int compression_timeout = COMPRESSION_TIMEOUT) const;

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

private:
	static QHash<int, Clients> clients;
	static QHash<int, OpenServerManager*> openservers;

	FrameCompressor frame_compressor, request_compressor;
};

#endif //__DEVICE_H__
