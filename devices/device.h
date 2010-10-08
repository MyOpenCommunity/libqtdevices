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


class FrameCompressor : public QObject
{
friend class TestDevice;
Q_OBJECT
public:
	FrameCompressor();

	void sendCompressedFrame(QString frame, int compression_timeout) const;

signals:
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


class device : public QObject, FrameReceiver
{
friend class TestDevice;
friend class BtMain;
Q_OBJECT

public:
	static const int COMPRESSION_TIMEOUT = 1000;

	virtual void init() { }

	virtual QString get_key();
	virtual ~device() {}

	static void setClients(const QHash<int, Clients> &c);

	virtual void manageFrame(OpenMsg &msg);

	bool isConnected();
	int openserverId();
	static void initDevices();

	static void sendCommandFrame(int openserver_id, const QString &frame);

	static void delayFrames(bool delay);

signals:
	void status_changed(QList<device_status*>);

	void valueReceived(const DeviceValues &values_list);

	void connectionUp();
	void connectionDown();

public slots:
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	void sendFrameNow(QString frame) const;
	void sendInitNow(QString frame) const;

	void sendCompressedFrame(QString frame, int compression_timeout = COMPRESSION_TIMEOUT) const;
	void sendCompressedInit(QString frame, int compression_timeout = COMPRESSION_TIMEOUT) const;

protected:
	device(QString who, QString where, int openserver_id = 0);

	QString who;
	QString where;

	int openserver_id;

	void sendCommand(QString what, QString _where) const;
	void sendCommand(QString what) const;
	void sendCommand(int what) const;
	void sendRequest(QString what) const;
	void sendRequest(int what) const;

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list) { return false; }

private:
	static OpenServerManager *getManager(int openserver_id);

private:
	static QHash<int, Clients> clients;
	static QHash<int, OpenServerManager*> openservers;

	FrameCompressor frame_compressor, request_compressor;
};

#endif //__DEVICE_H__
