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

#define AMPLI_NUM 100
#define BASE_EEPROM 11360
#define KEY_LENGTH 5
#define AL_KEY "\125\252\125\252\125"
#define SORG_PAR 2

class QString;


// for setVolume
enum VolumeType
{
    VOLUME_VIDEOCONTROL = 1,
    VOLUME_INTERCOM,
    VOLUME_MMDIFFUSION,
    VOLUME_BEEP,
    VOLUME_RING, //Suonerie
    VOLUME_FILE,
    VOLUME_VCTIP,
    VOLUME_MIC,
    VOLUME_MMDIFFUSIONSOURCE,
    VOLUME_MMDIFFUSIONAMPLIFIER
};

// Return the maximum width of the gui
int maxWidth();

// Return the maximum height of the gui
int maxHeight();

 /*!
  \brief Sets the contrast of the device.

  The contrast is set to the value of the first argument. If the second argument is TRUE also the configuration file is updated otherwise it isn't.
*/
void setContrast(unsigned char value);

 /*!
  \brief Gets the contrast of the device.
*/
unsigned char getContrast();

 /*!
  \brief Sets the backlight On/Off according to the value of the argument.
*/
void setBacklight(bool);

/**
 * Sets the backlight of the device on or off according to the value of the argument.
 * This differs from setBacklight in that it only sets the backlight (ie. writes only to
 * device /proc/sys/dev/btweb/brightness).
 * \param value If true, sets on the backlight.
 */
void setBacklightOn(bool value);

/**
 * Sets the brightness level of the display.
 * \param level The new brightness level. Allowed values are 0 <= level <= 255
 * Using values outside this range yields no results.
 */
void setBrightnessLevel(int level);

 /*!
  \brief Gets the backlight state.
*/
bool getBacklight();

/**
 * Enables the beep.
 * \param buzzer_enable Enables the beep for this execution of BTouch, without changing conf.xml.
 * \param write_to_conf Writes to config file the value passed in buzzer_enable.
 */
void setBeep(bool buzzer_enable);

 /*!
  \brief Sets the orientation of the device.
*/
void setOrientation(QString orientation);

 /*!
  \brief Makes a system beep.
*/
void beep();

 /*!
  \brief Makes a system beep and the argument represent the duration in ms.
*/
void beep(int);

 /*!
  \brief Retrieves if the beep is enabled or not.
*/
bool getBeep();

 /*!
  \brief Retrieves the time elapsed (in seconds) from last pressure of the device.
*/
unsigned long getTimePress();

#if defined(BT_HARDWARE_X11) || defined(BT_HARDWARE_TOUCHX)
class QDateTime;

// stores the time of the last click for getTimePress()
void setTimePress(const QDateTime &press);
#endif

 /*!
  \brief WatchDog rearm.
*/
void rearmWDT();

void getName(char *name);

 /*!
   \brief Reads alarm volume data
*/
void getAlarmVolumes(int index, int *volSveglia, uchar *sorgente, uchar *stazione);

 /*!
   \brief Stores alarm valume data
*/
void setAlarmVolumes(int index, int *volSveglia, uchar sorgente, uchar stazione);

// play a WAV file
void playSound(const QString &wavFile);
// stops a file previously played with playSound
void stopSound();

// set contrast value for entryphone
void setVctVideoValue(const QString &command, const QString &value);

// init multimedia playback
void initMultimedia();

// set the volume
void setVolume(VolumeType type, int value);

void initScreen();


#endif // HARDWARE_FUNCTIONS_H
