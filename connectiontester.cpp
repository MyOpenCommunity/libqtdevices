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


#include "connectiontester.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#define TIMEOUT_CONNECTION 5


ConnectionTester::ConnectionTester(QObject *parent) : QObject(parent)
{
	urls = QStringList() << "http://www.google.it" << "http://www.bticino.it";
	current_reply = NULL;
	manager = new QNetworkAccessManager(this);
	timeout_timer = new QTimer(this);
	timeout_timer->setSingleShot(true);
	timeout_timer->setInterval(TIMEOUT_CONNECTION * 1000);
	connect(timeout_timer, SIGNAL(timeout()), SLOT(downloadFailed()));
}

bool ConnectionTester::isTesting() const
{
	return current_reply != NULL;
}

void ConnectionTester::test()
{
	current_url = 0;
	startTest();
}

void ConnectionTester::cancel()
{
	current_reply->disconnect();
	current_reply->deleteLater();
	current_reply = NULL;
	timeout_timer->stop();
}

void ConnectionTester::startTest()
{
	QString url = urls.at(current_url);
	qDebug() << "start testing the connection with" << url;
	current_reply = manager->get(QNetworkRequest(QUrl(url)));
	connect(current_reply, SIGNAL(finished()), SLOT(downloadFinished()));
	timeout_timer->start();
}

void ConnectionTester::downloadFailed()
{
	qDebug() << "connection failed for" <<  urls.at(current_url);
	current_reply->disconnect();
	current_reply->deleteLater();
	if (current_url + 1 >= urls.size())
	{
		current_reply = NULL;
		emit testFailed();
		return;
	}
	++current_url;
	startTest();
}

void ConnectionTester::downloadFinished()
{
	timeout_timer->stop();
	if (current_reply->error() != QNetworkReply::NoError)
	{
		downloadFailed();
		return;
	}
	qDebug() << "connection established for" <<  urls.at(current_url);
	current_reply->disconnect();
	current_reply->deleteLater();
	current_reply = NULL;
	emit testPassed();
}


