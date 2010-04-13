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


#ifndef AMPLIFICATORI_H
#define AMPLIFICATORI_H

#include "bannregolaz.h"

#include <QWidget>
#include <QString>
#include <QList>

class device_status;
class device;
class AmplifierDevice;


class Amplifier : public BannLevel
{
Q_OBJECT
public:
	Amplifier(const QString &descr, const QString &where);

private:
	void setIcons();

private slots:
	void volumeUp();
	void volumeDown();
	void turnOn();
	void turnOff();

	void status_changed(const DeviceValues &status_list);

private:
	QString center_left_active, center_left_inactive, center_right_active, center_right_inactive;
	int volume_value;
	bool active;
	AmplifierDevice *dev;
};


class AmplifierGroup : public BannLevel
{
Q_OBJECT
public:
	AmplifierGroup(QStringList addresses, const QString &descr);

private slots:
	void volumeUp();
	void volumeDown();
	void turnOn();
	void turnOff();

private:
	QString center_left_active, center_left_inactive, center_right_active, center_right_inactive;
	QList<AmplifierDevice*> devices;
};

/*!
 * \class amplificatore
 * \brief This class is made to manage an audio amplifier.
 *
 * This class is quite similar to \a dimmer's one; it only has to deal with different \a Open \a messages.
 */
class amplificatore : public bannRegolaz
{
Q_OBJECT
public:
	amplificatore(QWidget *parent, QString indirizzo, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon);
	void inizializza(bool forza = false);
public slots:
	void status_changed(QList<device_status*>);
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
private:
	char value;
	device *dev;
};


/*!
 * \class grAmplificatori
 * \brief This class is made to manage a number of audio amplifier.
 *
 * It behaves essentially like \a amplificatore even if it doesn't represent the amplifiter's state since different
 * amplifiers can have different states.
 */
class grAmplificatori : public bannRegolaz
{
Q_OBJECT
public:
	grAmplificatori(QWidget *parent=0, QList<QString> indirizzi=QList<QString>(), QString IconaSx=QString(), QString IconaDx=QString(), QString Iconsx=QString(), QString Icondx=QString());
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
private:
	QList<QString> elencoDisp;
	void sendActivationFrame(QString argm);
};

#endif
