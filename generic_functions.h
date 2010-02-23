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


#include <QString>
#include <QList>
#include <QMap>

#include "main.h" // MY_FILE_USER_CFG_DEFAULT

/// Create a 'command' frame
/// This function is similar to openwebnet::CreateMsgOpen but unlike that the result
/// frame is not tested for correcteness.
QString createMsgOpen(QString who, QString what, QString where);

// Create a 'dimension request' frame
QString createRequestOpen(QString who, QString what, QString where);

// Create a 'write dimension request' frame
QString createWriteRequestOpen(QString who, QString what, QString where);

// Create a 'status request' frame
QString createStatusRequestOpen(QString who, QString where);

QString getBostikName(const QString &name, const QString &suffix);

 /*!
  \brief Transform a image file name into a pressed one.

  Return the filename of a image '.png' converted into a "p.png" file name.
*/
QString getPressName(QString name);

 /*!
  \brief Transform a image file name into a pressed one with a index taken from a \a antiintrusion \a zone name.

  Return the ".png" file name converted into a "[number]p.png" file name. The first argument is the name to convert, the second is a string like "zn" where n is the number of the \a antiintrusion \a zone.
*/
QString getZoneName(QString name, QString zone);

/*!
  Like getZoneName, but receives an "ambiente" number
*/
QString getAmbName(QString name, QString amb);

 /*!
  \brief Sets the contrast of the device.

  The contrast is set to the value of the first argument. If the second argument is TRUE also the configuration file is updated otherwise it isn't.
*/
void setContrast(unsigned char,bool);

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
void setBeep(bool buzzer_enable, bool write_to_conf);

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

 /*!
  \brief WatchDog rearm.
*/
void rearmWDT();

/**
 * Save one or more values in the configuration file related to a specific object.
 * \param data the map of the fields to save. The map has as key the path of the node (see
 * the function getElement to more details about the path) and as value the value that must
 * to be saved.
 * \param item_id the unique id of the object
 * \param num_item the serial number of the object
 */
bool setCfgValue(QMap<QString, QString> data, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

// A convenient overload for the above function
bool setCfgValue(QString field, QString value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

void getName(char *name);

int trasformaVol(int vol);

