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


#ifndef LAN_SETTINGS_H
#define LAN_SETTINGS_H

#include "page.h"

#include <QFrame>

class PlatformDevice;
class StateButton;
class QGridLayout;
class QDomNode;
class ConnectionTester;


// An utility class to show text in 2 column like a table.
class Text2Column : public QFrame
{
Q_OBJECT
public:
	Text2Column();
	void addRow(QString text, Qt::Alignment align=Qt::AlignLeft);
	void addRow(QString label, QString text);
	void setText(int row, QString text);
	void setSpacing(int spacing);

private:
	QGridLayout *main_layout;
	QTimer *timer;
};


/*!
	\ingroup Settings
	\brief Shows lan settings and enable/disable the lan interface.
*/
class LanSettings : public Page
{
Q_OBJECT
public:
	LanSettings(const QDomNode &config_node);
	virtual void inizializza();

public slots:
	virtual void showPage();

private slots:
	void valueReceived(const DeviceValues &values_list);
	void toggleLan();
	void connectionUp();
	void connectionDown();
	void handleClose();

private:
	StateButton *toggle_btn;
	Text2Column *box_text;
	PlatformDevice *dev;
	// The status of the lan (enable or disable)
	bool lan_status;
	// The status of the lan as stored in the configuration file
	bool saved_status;

	enum ConnectionStatus
	{
		UNKNOWN,
		UP,
		DOWN
	};

	ConnectionStatus connection_status;
	int attempts;
	int attempts_delay;

	int item_id;
	ConnectionTester *tester;


	void updateConnectionStatus();
	void startTest();
};

#endif // LAN_SETTINGS_H
