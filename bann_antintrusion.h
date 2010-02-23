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


#ifndef BANN_ANTINTRUSION_H
#define BANN_ANTINTRUSION_H

#include "bann1_button.h" // bannOnIcons
#include "bann3_buttons.h" // bann3ButLab
#include "device_status.h"

#include <QString>
#include <QList>

/// Forward Declarations
class Keypad;
class device;


/*!
 *  \class zonaAnti
 * \brief This class is made to manage an anti-intrusion zone.
 *
 * By this banner is possible to see if the zone is active or not in a certain moment.
 * \date lug 2005
 */
class zonaAnti : public bannOnIcons
{
using banner::SetIcons; // import the set of overload of SetIcons from banner
Q_OBJECT
public:
	zonaAnti(QWidget *parent, const QString &name, QString indirizzo, QString iconzona, QString IconDisactive, QString IconActive);
	void inizializza(bool forza = false);
	void Draw();
public slots:
	void status_changed(QList<device_status*>);
	void ToggleParzializza();
	void abilitaParz(bool);
	void clearChanged();
	int getIndex();
private:
	void SetIcons();
	QString parzIName, sparzIName;
	QString zonaAttiva;
	QString zonaNonAttiva;
	bool already_changed;
	device *dev;
signals:
	void partChanged(zonaAnti *);
};


/*!
 * \class impAnti
 * \brief This class is made to manage the anti-intrusion system.
 *
 * By this banner is possible to change the (dis)activation state of the system from the visualized one.
 * If there is an alarm queue it also possible to acces a page describing the it.
 * \date lug 2005
 */
class impAnti : public bann3ButLab
{
Q_OBJECT
public:
	impAnti(QWidget *parent, QString IconOn, QString IconOff, QString IconInfo, QString IconActive);
public slots:
	void status_changed(QList<device_status*>);
	void partChanged(zonaAnti*);
	void setZona(zonaAnti*);
	int getIsActive(int zona);
	void ToSendParz(bool s);
	void openAckRx();
	void openNakRx();
private:
	static const int MAX_ZONE = 8;
	Keypad *tasti;
	zonaAnti *le_zone[MAX_ZONE];
	bool send_part_msg;
	bool part_msg_sent;
	bool inserting;
	QString passwd;
	device *dev;
private slots:
	void inizializza(bool forza = false);
	void Inserisci();
	void Disinserisci();
	void Insert1();
	void Insert2();
	void Insert3();
	void DeInsert();
signals:
	void impiantoInserito();
	void abilitaParz(bool);
	void clearChanged();
	void clearAlarms();
};

#endif
