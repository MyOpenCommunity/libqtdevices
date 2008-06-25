/*!
 * \bannsettings.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNSETTINGS_H
#define BANNSETTINGS_H

#include "bann3but.h"
#include "bann4but.h"
#include "bannpuls.h"

class device;

/**
 * This banner shuts the thermal regulator off when clicked. Before using it, be sure to
 * set the address of the thermal regulator.
 * It displays a button in the center with icon "OFF".
 */
class BannOff : public bann3But
{
Q_OBJECT
public:
	BannOff(QWidget *parent, const char *name);
public slots:
	/**
	 * Shut down the thermal regulator
	 */
	void performAction();
private:
	/// The device that this banner sends commands to
	device *controlled_device;
};

/**
 * This banner sets the thermal regulator in antifreeze protection. Be sure to set the
 * address of the device.
 * It displays one button at the center with icon antifreeze on it.
 */
class BannAntifreeze : public bann3But
{
Q_OBJECT
public:
	BannAntifreeze(QWidget *parent, const char *name);
public slots:
	/**
	 * Set thermal regulator in antifreeze protection
	 */
	void performAction();
private:
	/// The device that this banner sends commands to
	device *controlled_device;
};

/**
 * This banner sets the thermal regulator in summer or winter status, depending on the
 * button pressed.
 * It displays two buttons at the center, one with the summer icon and one with the winter icon.
 */
class BannSummerWinter : public bann4But
{
Q_OBJECT
public:
	BannSummerWinter(QWidget *parent, const char *name);
	enum seasons {WINTER, SUMMER};
public slots:
	void setSummer();
	void setWinter();
private:
	seasons status;
	/// The device that this banner sends commands to
	device *controlled_device;
};

/**
 * This banner emits a signal with an int, corresponding to the program set with setProgram(). Default
 * program is 1, so be sure to set the program you want to be set before using it.
 * It displays a not clickable image on the center, an ok button on the right and program description
 * below.
 */
class BannWeekly : public bannPuls
{
Q_OBJECT
public:
	BannWeekly(QWidget *parent, const char *name);
	void setProgram(int prog);
public slots:
	void performAction();
private:
	int program;
signals:
	void programNumber(int);
};

#endif // BANNSETTINGS_H
