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


#ifndef DEVICEOLD_H
#define DEVICEOLD_H

#include "device.h"
#include "device_status.h"

class DeviceOld : public device
{
Q_OBJECT
public:
	virtual void init() { init(false); }

	virtual ~DeviceOld();
	virtual void manageFrame(OpenMsg &msg);

public slots:
	//! receive a frame
	virtual void frame_rx_handler(char *);
	//! Initialization requested by frame interpreter
	void init_requested_handler(QString msg);

signals:
	//! Invoked after successful initialization
	void initialized(device_status *);
	//! We want a frame to be handled
	void handle_frame(char *, QList<device_status*>);

protected:
	DeviceOld(QString who, QString where);

	//! Interpreter
	frame_interpreter *interpreter;
	//! List of device stats
	QList<device_status*> stat;

	/// connect the frame interpreter with the device
	void setup_frame_interpreter(frame_interpreter* i);

private:
	void init(bool force);
};


/********************* Specific class device children classes **********************/

//! MCI
class mci_device : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	mci_device(QString, bool p=false, int g=-1);
};


//! Sound device (ampli)
class sound_device : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	sound_device(QString, bool p=false, int g=-1);
};

//! Radio
class radio_device : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	radio_device(QString, bool p=false, int g=-1);
	virtual QString get_key();
};

//! Sound matrix
class sound_matr : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	sound_matr(QString, bool p=false, int g=-1);
};

//! Doorphone device
class doorphone_device : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	doorphone_device(QString, bool p=false, int g=-1);
};

//! ??
class impanti_device : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	impanti_device(QString, bool p=false, int g=-1);
};

//! ??
class zonanti_device : public DeviceOld
{
Q_OBJECT
public:
	//! Constructor
	zonanti_device(QString, bool p=false, int g=-1);
};

//! Aux device
class aux_device : public DeviceOld
{
Q_OBJECT
public:
	aux_device(QString w, bool p=false, int g=-1);
	virtual void init();

	virtual void manageFrame(OpenMsg &msg);

private:
	stat_var status;

signals:
	// TODO: rimpiazzare questo segnale con lo status_changed(QHash<>)..
	void status_changed(stat_var);
};


#endif // DEVICEOLD_H
