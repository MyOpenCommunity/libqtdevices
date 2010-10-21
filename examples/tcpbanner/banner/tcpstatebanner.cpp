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
#include "tcpstatebanner.h"

#include <QTcpSocket>

#include "skinmanager.h"


namespace
{
	const char *HOST = "localhost";
	const int PORT = 12345;
}

TcpStateBanner::TcpStateBanner(const QString &banner_name, const QStringList &states)
	: BannStates(0), name(banner_name), st(states)
{
	for (int i = 0; i < st.size(); ++i)
		addState(i, st.at(i));

	initBanner(bt_global::skin->getImage("forward"), 0);
	connect(this, SIGNAL(stateChanged(int)), SLOT(changeState(int)));
}

void TcpStateBanner::changeState(int state)
{
	sendMessage(name + ": " + st.at(state) + "\n");
}

void TcpStateBanner::sendMessage(const QString &message)
{
	QTcpSocket *socket = new QTcpSocket(this);

	socket->connectToHost(HOST, PORT);
	if (socket->waitForConnected())
		qDebug() << "Connected";

	socket->write(message.toAscii());
	if (socket->waitForBytesWritten())
		qDebug() << "Data written";

	socket->flush();

	socket->disconnectFromHost();
	socket->deleteLater();
}
