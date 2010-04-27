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


#ifndef GENERIC_FUNCTIONS_H
#define GENERIC_FUNCTIONS_H

#include "main.h" // MY_FILE_USER_CFG_DEFAULT, bt_global::config

#include <QString>
#include <QMap>

class QDate;
class QDateTime;
class OpenMsg;


/** The following functions can be used to test the format of a frame or build it.
 *  The frame format can be:
 *  - a command frame, in the form *who*what*where##
 *  - a read dimension frame, in the form *#who*where*dimension##
 *  - a write dimension frame, in the form *#who*where*#dimension*val1*..*valn##
 *  - a status request frame, in the form *#who*where##
 */

bool isCommandFrame(OpenMsg &msg);
bool isDimensionFrame(OpenMsg &msg);
bool isWriteDimensionFrame(OpenMsg &msg);
bool isStatusRequestFrame(OpenMsg &msg);

QString createCommandFrame(QString who, QString what, QString where);
QString createDimensionFrame(QString who, QString dimension, QString where);
QString createWriteDimensionFrame(QString who, QString dimension, QString where);
QString createStatusRequestFrame(QString who, QString where);

QStringList getImageFileFilter();


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

/**
 * Save one or more values in the configuration file related to a specific object.
 * \param data the map of the fields to save. The map has as key the path of the node (see
 * the function getElement to more details about the path) and as value the value that must
 * to be saved.
 *
 * As a convenience, the function is a no-op during constructions and configuration loading
 * and starts actually writing to the file just before page initialization
 *
 * \param item_id the unique id of the object
 * \param num_item the serial number of the object
 */
#ifdef CONFIG_BTOUCH

bool setCfgValue(QMap<QString, QString> data, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

// Some convenient overloads for the above function
bool setCfgValue(QString field, QString value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);
bool setCfgValue(QString field, int value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

// sets config values for nodes that do not have an id
bool setGlobalCfgValue(QMap<QString, QString> data, const QString &id_name, int id_value, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#else

bool setCfgValue(QMap<QString, QString> data, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

// Some convenient overloads for the above function
bool setCfgValue(QString field, QString value, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);
bool setCfgValue(QString field, int value, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#endif

void getName(char *name);

int trasformaVol(int vol);


/**
 * This namespace contains the functions to convert a QDate/QDateTime in a string
 * (and viceversa) formatted as specified in the configuration file.
 */
namespace DateConversions
{
	QString formatDateConfig(const QDate &date, char separator = '.');
	QString formatDateTimeConfig(const QDateTime &datetime, char separator = '.');
	QDate getDateConfig(const QString &date, char separator = '.');
	QDateTime getDateTimeConfig(const QString &datetime, char separator = '.');
}

#endif // GENERIC_FUNCTIONS_H
