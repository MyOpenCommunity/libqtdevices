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


#ifndef FRAME_RECEIVER_H
#define FRAME_RECEIVER_H

#include <QObject>
#include <QHash>

class Client;
class OpenMsg;


class FrameReceiver
{
public:
	FrameReceiver(int openserver_id = 0);
	virtual ~FrameReceiver();

	static void setClientsMonitor(const QHash<int, Client*> &monitors);

	virtual void manageFrame(OpenMsg &msg) = 0;

protected:
	void subscribe_monitor(int who);

	static QHash<int, Client*> clients_monitor;

private:
	bool subscribed;
	int openserver_id;
};


#endif // FRAME_RECEIVER_H
