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


#ifndef HARDWARE_FUNCTIONS_H
#define HARDWARE_FUNCTIONS_H

#include <QtGlobal> // uchar
#include <QPair>
#include <QString>

#define DEV_E2 "/dev/nvram"

#define AMPLI_NUM 100

class QProcess;


/*!
	\ingroup Core
	\brief Return the maximum width of the gui
*/
int maxWidth();

/*!
	\ingroup Core
	\brief Return the maximum height of the gui
*/
int maxHeight();

/*
	Sets the contrast.

	The contrast is set to the value of the first argument. If the second argument
	is true also the configuration file is updated otherwise it isn't.
*/
void setContrast(unsigned char value);

/*
	Gets the contrast of the device.
*/
unsigned char getContrast();

/*!
	\ingroup Core
	\brief Sets the backlight of the device on or off according to the value of the argument.
*/
void setBacklight(bool value);

/*!
	\ingroup Core
	\brief Sets the brightness level of the display.

	The \a level is the new brightness level. Allowed values are 1 <= level <= 10.
	Using values outside this range yields no results.
*/
void setBrightnessLevel(int level);

// Gets the backlight state.
bool getBacklight();

// Enables the beep
void setBeep(bool enable);

// Sets the orientation of the device (used only for TS 3.5)
void setOrientation(QString orientation);

/*!
	\ingroup Core
	\brief Makes a system beep.
*/
void beep();

// Makes a system beep and the argument represent the duration in ms.
void beep(int);

// Retrieves if the beep is enabled or not.
bool getBeep();

// Retrieves the time elapsed (in seconds) from last pressure of the touchscreen.
unsigned long getTimePress();

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TS_10)
class QDateTime;

// stores the time of the last click for getTimePress()
void setTimePress(const QDateTime &press);
#endif

/*!
	\ingroup Core
	\brief Rearm the WatchDog software.

	On the touchscreens 10'' and 3.5'' there is a watchdog software to ensure
	that the software is actually running. Call this method to avoid a touchscreen
	reboot.
*/
void rearmWDT();

void getName(char *name);

// Reads alarm volume data.
void getAlarmVolumes(int index, int *volSveglia, uchar *sorgente, uchar *stazione);

// Stores alarm valume data.
void setAlarmVolumes(int index, int *volSveglia, uchar sorgente, uchar stazione);

// Return the pair command, arguments to reproduce the audio file argument.
QPair <QString, QStringList> getAudioCmdLine(const QString &audio_path);

// set contrast value for entryphone
void setVctVideoValue(const QString &command, const QString &value);

// init multimedia playback
void initMultimedia();

// mount usb devices at boot (if present)
void usbHotplug();

// Dump (on the debug channel) some statistic about the status of the memory
void dumpSystemMemory();

// Return the available memory in KB (-1 if the information is not available)
int getMemFree();

// Create a file used as a flag to notify something to the other openserver stack processes.
void createFlagFile(QString filename);

#endif // HARDWARE_FUNCTIONS_H
