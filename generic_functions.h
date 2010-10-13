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

#include <QStringList>
#include <QString>
#include <QMap>

class QDate;
class QDateTime;


enum MultimediaFileType
{
	UNKNOWN = -1,
	DIRECTORY = 0,
	AUDIO = 1,
	VIDEO = 2,
	IMAGE = 3
};

QStringList getFileExtensions(MultimediaFileType type);

QStringList getFileFilter(MultimediaFileType type);

QString getBostikName(const QString &name, const QString &suffix);

QString getPressName(QString name);


/**
 * Save one or more values in the configuration file related to a specific object.
 * \param data the map of the fields to save. The map has as key the path of the node (see
 * the function getElement to more details about the path) and as value the value that must
 * to be saved.
 *
 * As a convenience, the function is a no-op during constructions and configuration loading
 * and starts actually writing to the file just before page initialization
 *
 * Note that the actual file writes are batched and performed some time after the set[Global]CfgValue call.
 *
 * \param item_id the unique id of the object
 * \param num_item the serial number of the object
 */
#ifdef CONFIG_BTOUCH

void setCfgValue(QMap<QString, QString> data, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

// Some convenient overloads for the above function
void setCfgValue(QString field, QString value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);
void setCfgValue(QString field, int value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#else

void setCfgValue(QMap<QString, QString> data, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

// Some convenient overloads for the above function
void setCfgValue(QString field, QString value, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);
void setCfgValue(QString field, int value, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#endif

// sets config values for nodes that do not have an id. We have to specify a root node to avoid duplicates.
void setGlobalCfgValue(const QString &root_name, QMap<QString, QString> data, const QString &tag_name, int id_value, const QString &filename=MY_FILE_USER_CFG_DEFAULT);


int localVolumeToAmplifier(int vol);
int scsToLocalVolume(int vol);


/**
 * This namespace contains the functions to convert a QDate/QDateTime in a string
 * (and viceversa) formatted as specified in the configuration file.
 */
namespace DateConversions
{
#ifdef CONFIG_BTOUCH
	const char separator = '.';
#else
	const char separator = '/';
#endif

	QString formatDateConfig(const QDate &date, char s = separator);
	QString formatDateTimeConfig(const QDateTime &datetime, char s = separator);
	QDate getDateConfig(const QString &date, char s = separator);
	QDateTime getDateTimeConfig(const QString &datetime, char s = separator);
}

// A wrapper around the QProcess::execute that show the time elapsed in DEBUG mode
bool smartExecute(const QString &program, QStringList args = QStringList());

// A wrapper around the QProcess::execute that show the time elapsed in DEBUG mode appending
// some args to the process in order to silent its output & error messages.
bool silentExecute(const QString &program, QStringList args = QStringList());

// Returns true if the image can be loaded without exausting the ram
bool checkImageLoad(const QString &path);

#endif // GENERIC_FUNCTIONS_H
