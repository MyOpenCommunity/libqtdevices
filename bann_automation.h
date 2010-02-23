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


#ifndef BANN_AUTOMATION_H
#define BANN_AUTOMATION_H

#include "bann1_button.h" // bannPuls
#include "bann3_buttons.h" // bann3But
#include "bann2_buttons.h" // bannOnOff
#include "device.h"        // StatusList

#include <QWidget>
#include <QString>

/// Forward Declarations
class device;
class device_status;
class PPTStatDevice;
class AutomationDevice;
class LightingDevice;
class QDomNode;


class InterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	InterblockedActuator(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza = false);

private slots:
	void sendGoUp();
	void sendGoDown();
	void sendStop();
	void status_changed(const StatusList &sl);

private:
	void connectButton(BtButton *btn, const char *slot);
	AutomationDevice *dev;
};

/**
 * Send open/close frame on button press and send stop on button release.
 *
 * Banner behaviour must change (both graphically and logically) depending on who was the origin of the action.
 * In case of update from bus, the icon must be STOP not pressed and a stop frame must be sent when the button
 * is clicked.
 * In case of command sent after the user pressed the touch screen, the icon must be STOP pressed and a stop
 * frame must be sent on button release.
 */
class SecureInterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	SecureInterblockedActuator(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza = false);

private slots:
	void sendOpen();
	void sendClose();
	void buttonReleased();
	void sendStop();
	void status_changed(const StatusList &sl);

private:
	void connectButtons();
	void changeButtonStatus(BtButton *btn);
	AutomationDevice *dev;
	bool is_any_button_pressed;
};

class GateEntryphoneActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateEntryphoneActuator(QWidget *parent, const QDomNode &config_node);

private slots:
	void activate();

private:
	QString where;
	device *dev;
};

class GateLightingActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateLightingActuator(QWidget *parent, const QDomNode &config_node);

private slots:
	void activate();

private:
	int time_h, time_m, time_s;
	LightingDevice *dev;
};


class InterblockedActuatorGroup : public Bann3Buttons
{
Q_OBJECT
public:
	InterblockedActuatorGroup(QWidget *parent, const QDomNode &config_node);

private slots:
	void sendOpen();
	void sendClose();
	void sendStop();

private:
	QList<AutomationDevice *> actuators;
};

#if 0
/*!
 * \class
 * \brief This class represents an automated video-doorphone actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatVC : public bannPuls
{
Q_OBJECT
public:
	automCancAttuatVC(QWidget *parent, QString where, QString IconaSx, QString IconaDx);
private:
	device *dev;
private slots:
	void Attiva();
};


/*!
 * \class
 * \brief This class represents an automated light actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatIll : public bannPuls
{
Q_OBJECT
public:
	automCancAttuatIll(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString t);
private:
	device *dev;
	QString time;
private slots:
	void Attiva();
};


/*!
 * \class attuatAutomIntSic
 * \brief This class is made to manage a secure interblocked actuator.
 *
 * This kind of actuators controls gates, stretches and so on. Pushing on the right button the object closes while releasing
 * the same button the automation stops. On the left-button there's the same behavior with the difference that the object
 * is opened.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomIntSic : public bannOnOff
{
Q_OBJECT
public:
	attuatAutomIntSic(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive, QString IconDisactive);
	virtual void inizializza(bool forza = false);
public slots:
	virtual void status_changed(QList<device_status*>);
private:
	char uprunning ,dorunning;
	QString icon_sx, icon_dx, icon_stop;
	device *dev;
private slots:
	void doPres();
	void upPres();
	void doRil();
	void upRil();
	void sendStop();
};


/*!
 * \class attuatAutomInt
 * \brief This class is made to manage an interblocked actuator.
 *
 * This kind of actuators controls gates, stretches and so on. Clicking the right button the object closes, and on the button
 * appears a stop image. A new pressure cause the stop af the automation. On the left-button there's the same behavior with
 * the difference that the object is opened.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomInt : public bannOnOff
{
Q_OBJECT
public:
	attuatAutomInt(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive, QString IconDisactive);
	virtual void inizializza(bool forza = false);
public slots:
	virtual void status_changed(QList<device_status*>);
private slots:
	void analizzaUp();
	void analizzaDown();
private:
	char uprunning,dorunning;
	QString icon_sx, icon_dx, icon_stop;
	device *dev;
};


/*!
 * \class grAttuatInt
 * \brief This class is made to manage a number of  interblocked actuators.
 *
 * Clicking the right button the objects close,  clicking the left-button the objects open while clicking the middle 
 * button the objects stop.
 * \author Davide
 * \date lug 2005
 */
class grAttuatInt : public bann3But
{
Q_OBJECT
public:
	grAttuatInt(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString Icon);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
public slots:
	void Alza();
	void Abbassa();
	void Ferma();
private:
	QList<QString> elencoDisp;
	void sendAllFrames(QString msg);
};
#endif


class PPTStat : public banner
{
Q_OBJECT
public:
	PPTStat(QWidget *parent, QString where, int cid);
	virtual void inizializza(bool forza=false);

private:
	PPTStatDevice *dev;
	QString img_open, img_close;
private slots:
	void status_changed(const StatusList &status_list);
};

#endif
