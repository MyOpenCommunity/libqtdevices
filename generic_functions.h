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


 /*!
	\brief Filesystem types.
 */
enum MultimediaFileType
{
	UNKNOWN = -1,  /*!< Unknown filetype */
	DIRECTORY = 0, /*!< Directory */
	AUDIO = 1,     /*!< Audio filetype */
	VIDEO = 2,     /*!< Video filetype */
	IMAGE = 3,     /*!< Image filetype */
	PDF   = 4,     /*!< PDF filetype */
};

/*!
	\ingroup Core
	\brief Returns a list of the recognized file types associated to \a type.
 */
QStringList getFileExtensions(MultimediaFileType type);

/*!
	\ingroup Core
	\brief Returns a list of file filter expressions associated to \a type.
 */
QStringList getFileFilter(MultimediaFileType type);

/*!
	\ingroup Core
	\brief Concatenate \a suffix to the part of \a name preceding the dot (.)
 */
QString getBostikName(const QString &name, const QString &suffix);

/*!
	\ingroup Core
	\brief Transform a image file name into a pressed one.

	Returns the filename of a image '.png' converted into a "p.png" file name.

	\sa getBostikName()
*/
QString getPressName(QString name);


#ifdef CONFIG_TS_3_5

/*!
	\ingroup Core
	\brief Save one or more values in the configuration file related to a specific object.

	This is an overloaded member function, provided for convenience.
	\note This function is for compatibility with the old configuration file of touchscreen 3.5''.
*/
void setCfgValue(QMap<QString, QString> data, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

/*!
	\ingroup Core
	\brief Save one or more values in the configuration file related to a specific object.

	This is an overloaded member function, provided for convenience.
	\note This function is for compatibility with the old configuration file of touchscreen 3.5''.
*/
void setCfgValue(QString field, QString value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

/*!
	\ingroup Core
	\brief Save one or more values in the configuration file related to a specific object.

	This is an overloaded member function, provided for convenience.
	\note This function is for compatibility with the old configuration file of touchscreen 3.5''.
*/
void setCfgValue(QString field, int value, int item_id, int num_item=1, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#else

/*!
	\ingroup Core
	\brief Save one or more values in the configuration file related to a specific object.

	\a data is the map of the fields to save. The map has as key the path of the
	node (see getElement() to more details about the path) and as value the value
	that must to be saved.

	As a convenience, the function is a no-op during constructions and configuration loading
	and starts actually writing to the file just before page initialization

	For performance reasons the actual file writes are batched and performed
	some time after the setCfgValue() / setGlobalCfgValue() call.
	The \a item_id is the unique id of the object.
*/
void setCfgValue(QMap<QString, QString> data, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

/*!
	\ingroup Core
	\brief Save one or more values in the configuration file related to a specific object.

	This is an overloaded member function, provided for convenience.
*/
void setCfgValue(QString field, QString value, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

/*!
	\ingroup Core
	\brief Save one or more values in the configuration file related to a specific object.

	This is an overloaded member function, provided for convenience.
*/
void setCfgValue(QString field, int value, int item_id, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#endif

/*!
	\ingroup Core
	\brief Sets config values for nodes that do not have an id.

	\note We have to specify a root node to avoid duplicates.
*/
void setGlobalCfgValue(const QString &root_name, QMap<QString, QString> data, const QString &tag_name, int id_value, const QString &filename=MY_FILE_USER_CFG_DEFAULT);

#ifdef LAYOUT_TS_10

/*!
	\ingroup Core
	\brief Converts a volume \a vol expressed in the local format in the scs format.

	Converts a local volume \a vol (range from 0 to 8) to the scs format (from 0 to 31).
*/
int localVolumeToAmplifier(int vol);

/*!
	\ingroup Core
	\brief Converts an scs volume \a vol in the local format.
	\sa localVolumeToAmplifier()
*/
int scsToLocalVolume(int vol);

#endif

/*!
	\ingroup Core
	\brief Converts an scs volume \a vol to the range used for the UI.
*/
int scsToGraphicalVolume(int vol);

/*!
	Contains the functions to convert a QDate/QDateTime in a string (and viceversa)
	formatted as specified in the configuration file.
*/
namespace DateConversions
{

	/*!
		\ingroup Core
		\brief The default separator used in dates

		It corresponds to '.' on the 3.5" touchscreen, '/' on the 10" one.
	*/
#ifdef CONFIG_TS_3_5
	const char separator = '.';
#else
	const char separator = '/';
#endif

	/*!
		\ingroup Core
		\brief Converts \a date to a QString object according to the date format read
		from configuration.
	*/
	QString formatDateConfig(const QDate &date, char s = separator);

	/*!
		\ingroup Core
		\brief Converts \a datetime to a QString object according to the datetime
		format read from configuration.
	*/
	QString formatDateTimeConfig(const QDateTime &datetime, char s = separator);

	/*!
		\ingroup Core
		\brief Converts \a date into a QDate object using the format read from the
		configuration.
	*/
	QDate getDateConfig(const QString &date, char s = separator);

	/*!
		\ingroup Core
		\brief Converts \a datetime into a QDateTime object using the format read
		from the configuration.
	*/
	QDateTime getDateTimeConfig(const QString &datetime, char s = separator);
}

/*!
	\ingroup Core
	\brief A wrapper around the QProcess::execute that shows the time elapsed in DEBUG mode.
*/
bool smartExecute(const QString &program, QStringList args = QStringList());

/*!
	\ingroup Core
	\brief A \em silent wrapper around the QProcess::execute.

	Shows the time elapsed in DEBUG mode and appends some extra args to the process
	in order to silent its output & error messages.
*/
bool silentExecute(const QString &program, QStringList args = QStringList());

/*!
	\ingroup Core
	\brief Returns true if the image can be loaded without exausting the ram
*/
//
bool checkImageLoad(const QString &path);

#endif // GENERIC_FUNCTIONS_H
