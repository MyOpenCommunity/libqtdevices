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
#include "tcpdimmer.h"

#include <QTcpSocket>

#include "skinmanager.h"
#include "btbutton.h"

namespace
{
	const char *HOST = "localhost";
	const int PORT = 12345;
}

TcpDimmer::TcpDimmer(const QString &banner_name, const QString &descr) :
	AdjustDimmer(0), name(banner_name), light_value(0)
{
	light_value = 0;
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("dimmer"),
		bt_global::skin->getImage("dimmer"), bt_global::skin->getImage("on"),
		bt_global::skin->getImage("dimmer_broken"), OFF, light_value, descr);

	connect(right_button, SIGNAL(clicked()), SLOT(lightOn()));
	connect(left_button, SIGNAL(clicked()), SLOT(lightOff()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(decreaseLevel()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(increaseLevel()));
}

void TcpDimmer::lightOn()
{
	setState(ON);
	setLevel(light_value);
	sendMessage(name + ": Light ON\n");
}

void TcpDimmer::lightOff()
{
	setState(OFF);
	setLevel(light_value);
	sendMessage(name + ": Light OFF\n");
}

void TcpDimmer::increaseLevel()
{
	sendMessage(QString(name + ": Light level %1\n").arg(++light_value));
}

void TcpDimmer::decreaseLevel()
{
	sendMessage(QString(name + ": Light level %1\n").arg(--light_value));
}

void TcpDimmer::sendMessage(const QString &message)
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
