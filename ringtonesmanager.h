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


#ifndef RINGTONESMANAGER_H
#define RINGTONESMANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QHash>


enum RingtoneType
{
	RINGTONE_PE1, // call from external unit with ringtone 1
	RINGTONE_PE2,
	RINGTONE_PE3,
	RINGTONE_PE4,
	RINGTONE_PI_INTERCOM,
	RINGTONE_PE_INTERCOM
};


/**
 * Manages ringtones globally.
 *
 * Load/store ringtone preferences from/to flash, play ringtones when needed.
 */
class RingtonesManager : public QObject
{
Q_OBJECT
public:
	RingtonesManager();

	/**
	 * Play the current ringtone set for the given ringtone type.
	 */
	void playRingtone(RingtoneType t);
	// play the ringtone set for the given id
	void playRingtone(int ring);
	// stops playing a ringtone, if any
	void stopRingtone();

	/**
	 * Set a new ringtone for a given ringtone type.
	 */
	void setRingtone(RingtoneType t, int ring);

	/**
	 * Return the current ringtone for a given type.
	 */
	int getRingtone(RingtoneType t);

	int getRingtonesNumber();

private:
	QList<QFileInfo> scanDirectory(const QDir &dir, const QStringList &filters);
	QString typeToFilePath(RingtoneType t);
	QHash<int, QString> ringtone_to_file;
	QHash<RingtoneType, int> type_to_ringtone;
};

namespace bt_global { extern RingtonesManager *ringtones; };

#endif // RINGTONESMANAGER_H
