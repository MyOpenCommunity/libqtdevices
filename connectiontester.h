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


#ifndef CONNECTIONTESTER_H
#define CONNECTIONTESTER_H

#include <QObject>
#include <QStringList>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;


/*
 * An helper class that tests the connection status, trying to download a list
 * of web pages from internet. It emits testFailed/testPassed to notify
 * the test result.
 */
class ConnectionTester : public QObject
{
	Q_OBJECT
public:
	ConnectionTester(QObject *parent);
public slots:
	// Start a new test
	void test();
	// Check if a test is currently running
	bool isTesting() const;
	// Abort the test
	void cancel();

signals:
	void testFailed();
	void testPassed();

private slots:
	void downloadFailed();
	void downloadFinished();

private:
	QNetworkAccessManager *manager;
	QNetworkReply *current_reply;
	QTimer *timeout_timer;
	QStringList urls;
	int current_url;
	void startTest();
};

#endif // CONNECTIONTESTER_H
