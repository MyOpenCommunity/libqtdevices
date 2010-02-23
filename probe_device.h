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


#ifndef PROBE_DEVICE_H
#define PROBE_DEVICE_H

#include "device.h"

class OpenMsg;


class NonControlledProbeDevice : public device
{
Q_OBJECT
public:
	enum ProbeType
	{
		INTERNAL = 0,
		EXTERNAL = 1
	};

	enum Type
	{
		DIM_TEMPERATURE = 1,
	};

	NonControlledProbeDevice(QString where, ProbeType type);

	void requestStatus();

	virtual void frame_rx_handler(char *frame);

protected:
	virtual void manageFrame(OpenMsg &msg);

private:
	ProbeType type;
};

#endif // PROBE_DEVICE_H

