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

class SoundPlayer;

namespace Ringtones
{
	/*!
		\ingroup Core
		\brief The types of ringtones.

		A ringtone type defines a list of logical event for which a ringtone should
		be played.

		\sa VideoDoorEntryDevice
	*/
	enum Type
	{
		PE1 = 1,
		PE2,
		PE3,
		PE4,
		PI_INTERCOM,
		PE_INTERCOM,
		FLOORCALL,
		ALARM,
		MESSAGE
	};
}

/*!
	\ingroup Core
	\brief %Loads and play ringtones.

	%Loads the ringtones defined in an xml file, sets a ringtone for a
	Ringtones::Type and plays/stops a ringtone.
*/
class RingtonesManager : public QObject
{
Q_OBJECT
public:
	RingtonesManager(QString ringtone_file);

	/*!
		\brief Play the current ringtone set for the given ringtone type \a t.
	*/
	void playRingtone(Ringtones::Type t);

	// play the ringtone set for the given id
	void playRingtone(int ring);

	/*!
		\brief stops playing a ringtone, if any
	*/
	void stopRingtone();

	/*!
		Sets a new ringtone for the given ringtone type \a t.
	*/
	void setRingtone(Ringtones::Type t, int item_id, int ring);

	// Ringtones starts from 1 to... ringtones_number
	int getRingtonesNumber();

signals:
	void ringtoneFinished();

private:
	QHash<int, QString> ringtone_to_file;
	QHash<Ringtones::Type, int> type_to_ringtone;
	SoundPlayer *sound_player;
};

namespace bt_global { extern RingtonesManager *ringtones; }

/*! \file */

#endif // RINGTONESMANAGER_H
