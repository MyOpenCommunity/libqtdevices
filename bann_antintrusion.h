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

#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // bann3ButLab
#include "device_status.h"

#include <QString>
#include <QList>

/// Forward Declarations
class Keypad;
class device;
class QDomNode;


class BannSingleLeft : public Bann2Buttons
{
Q_OBJECT
protected:
	enum States
	{
		PARTIAL_ON,
		PARTIAL_OFF,
	};

	BannSingleLeft();
	void initBanner(const QString &_left_on, const QString &_left_off,
			States init_state, const QString &banner_text);
	void setState(States new_state);

private:
	QString left_on, left_off;
};



// TODO: quick replacement for zonaAnti, to be cleaned up
class AntintrusionZone : public BannSingleLeft
{
Q_OBJECT
public:
	AntintrusionZone(const QString &name, const QString &_where);
	void inizializza(bool forza = false);
	bool isActive();

public slots:
	void status_changed(QList<device_status*>);
	void toggleParzializza();
	void abilitaParz(bool ab);
	void clearChanged();
	int getIndex();

private:
	void setParzializzaOn(bool parz);
	void updateButtonState();

	bool already_changed;
	bool is_on, is_partial;
	device *dev;
	QString where;
	QString left_disabled_on, left_disabled_off;

signals:
	void partChanged(AntintrusionZone *);
};

#if 0

typedef class zonaAnti AntintrusionZone;

/*!
 *  \class zonaAnti
 * \brief This class is made to manage an anti-intrusion zone.
 *
 * By this banner is possible to see if the zone is active or not in a certain moment.
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
	void partChanged(AntintrusionZone *);
};

#endif

/*!
 * \class impAnti
 * \brief This class is made to manage the anti-intrusion system.
 *
 * By this banner is possible to change the (dis)activation state of the system from the visualized one.
 * If there is an alarm queue it also possible to acces a page describing the it.
 */
class impAnti : public bann3ButLab
{
Q_OBJECT
public:
	impAnti(QWidget *parent, QString IconOn, QString IconOff, QString IconInfo, QString IconActive);

public slots:
	void status_changed(QList<device_status*>);
	void partChanged(AntintrusionZone *);
	void setZona(AntintrusionZone *);
	int getIsActive(int zona);
	void ToSendParz(bool s);
	void openAckRx();
	void openNakRx();
	void inizializza(bool forza = false);

signals:
	void impiantoInserito();
	void abilitaParz(bool);
	void clearChanged();
	void clearAlarms();

private:
	static const int MAX_ZONE = 8;
	Keypad *tasti;
	AntintrusionZone *le_zone[MAX_ZONE];
	bool send_part_msg;
	bool part_msg_sent;
	bool inserting;
	QString passwd;
	device *dev;

private slots:
	void Inserisci();
	void Disinserisci();
	void Insert1();
	void Insert2();
	void Insert3();
	void DeInsert();
};

#endif
