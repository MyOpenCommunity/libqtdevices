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


#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "bann2_buttons.h" // bannOnOff
#include "bann1_button.h" // bannPuls

class device;
class device_status;
class LightingDevice;
class QDomNode;


class SingleActuator : public BannOnOffState
{
Q_OBJECT
public:
	SingleActuator(QWidget *parent, const QDomNode &config_node, QString address);
	virtual void inizializza(bool forza = false);

private slots:
	void activate();
	void deactivate();
	void status_changed(const StatusList &status_list);

private:
	LightingDevice *dev;
};

class ButtonActuator : public BannSinglePuls
{
Q_OBJECT
public:
	ButtonActuator(QWidget *parent, const QDomNode &config_node, int type);

private slots:
	void activate();
	void deactivate();

private:
	int type;
};


#if 0
/*!
 * \class attuatAutom
 * \brief This is the \a automation \a actuator-banner class.
 * This class is used to represent both lightings and various kind of automation such as fan and irrigation.
 * \author Davide
 * \date lug 2005
 */
class attuatAutom : public bannOnOff
{
Q_OBJECT

public:
	attuatAutom(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive,
		QString IconDisactive);
public slots:
	void status_changed(QList<device_status*>);
private:
	char gruppi[4];
	device *dev;
	void inizializza(bool forza=false);
private slots:
	void Attiva();
	void Disattiva();
};


/*!
 * \class attuatPuls
 * \brief This class is made to manage a pulse automation.
 *
 * This object is useful when there's the need to activate something only during the pressure time 
 * (for instance when dealing with a lock).
 * \author Davide
 * \date lug 2005
 */
class attuatPuls : public bannPuls
{
Q_OBJECT
public:
	attuatPuls(QWidget *parent, QString where, QString IconaSx, QString IconActive, char tipo);
private slots:
	void Attiva();
	void Disattiva();
private:
	char type;
};
#endif

#endif
