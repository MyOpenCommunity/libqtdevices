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


#ifndef BANN_LIGHTING_H
#define BANN_LIGHTING_H

#include "bannregolaz.h" // BannLevel
#include "bann2_buttons.h" // BannOnOffNew, BannOnOff2Labels
#include "bann1_button.h" // BannOn2Labels
#include "bttime.h" // BtTime
#include "device.h" // DeviceValues

#include <QList>
#include <QTimer>


class device;
class device_status;
class openwebnet;
class LightingDevice;
class DimmerDevice;
class Dimmer100Device;


class LightGroup : public Bann2Buttons
{
Q_OBJECT
public:
	LightGroup(const QList<QString> &addresses, const QString &descr);

private slots:
	void lightOn();
	void lightOff();

private:
	// of course these pointers must not be deleted since objects are owned by
	// the cache
	QList<LightingDevice *> devices;
};


/*
 * A derived class of BannLevel which handles all dimmer state changes (both levels and states).
 */
class AdjustDimmer : public BannLevel
{
Q_OBJECT
protected:
	enum States
	{
		ON,
		OFF,
		BROKEN,
	};
	AdjustDimmer(QWidget *parent);
	void initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &broken, States init_state, int init_level,
		const QString &banner_text);
	void setLevel(int level);
	void setState(States new_state);

private:
	void setOnIcons();
	int current_level;
	States current_state;
	QString center_left, center_right, broken;
};


class Dimmer : public AdjustDimmer
{
Q_OBJECT
public:
	Dimmer(const QString &descr, const QString &where, int openserver_id);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();
	void valueReceived(const DeviceValues &values_list);

private:
	DimmerDevice *dev;
	int light_value;
};


class DimmerGroup : public BannLevel
{
Q_OBJECT
public:
	DimmerGroup(const QList<QString> &addresses, const QString &descr);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	QList<DimmerDevice *> devices;
};


class Dimmer100 : public AdjustDimmer
{
Q_OBJECT
public:
	Dimmer100(const QString &descr, const QString &where, int openserver_id, int _start_speed, int _stop_speed);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();
	void valueReceived(const DeviceValues &values_list);

private:
	int roundTo5(int value);
	Dimmer100Device *dev;
	int start_speed, stop_speed;
	int light_value;
};


class Dimmer100Group : public BannLevel
{
Q_OBJECT
public:
	Dimmer100Group(const QList<QString> &addresses, const QList<int> start_values, const QList<int> stop_values, const QString &descr);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	QList<Dimmer100Device *> devices;
	QList<int> start_speed, stop_speed;
};


class TempLight : public BannOnOff2Labels
{
Q_OBJECT
public:
	TempLight(const QString &descr, const QString &where, int openserver_id);

protected:
	void updateTimeLabel();

	QList<BtTime> times;
	int time_index;
	LightingDevice *dev;

protected slots:
	virtual void activate();

private slots:
	void valueReceived(const DeviceValues &values_list);
	void cycleTime();
};


class TempLightVariable : public TempLight
{
Q_OBJECT
public:
	TempLightVariable(const QList<BtTime> &time_values, const QString &descr, const QString &where, int openserver_id);

protected slots:
	virtual void activate();
};


class TempLightFixed : public BannOn2Labels
{
Q_OBJECT
public:
	TempLightFixed(int time, const QString &descr, const QString &where, int openserver_id);

private slots:
	void valueReceived(const DeviceValues &values_list);
	void setOn();
	void updateTimerLabel();

private:
	void stopTimer();
	int total_time;
	// we have received a valid variable timing update since last request
	bool valid_update;
	// retries when no response to a variable timing request has been received
	int update_retries;

	BtTime lighting_time;
	QTimer request_timer;
	LightingDevice *dev;
};

#endif
