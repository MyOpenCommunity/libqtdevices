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


#include "ringtonesmanager.h"
#include "hardware_functions.h" // playSound, stopSound
#include "xml_functions.h" // getChildren, getTextChild
#include "generic_functions.h" // setCfgValue

#include <QStringList>
#include <QDomDocument>
#include <QDebug>
#include <QDir>



RingtonesManager::RingtonesManager(QString ringtone_file)
{
	QFile fh(ringtone_file);
	QDomDocument qdom;

	if (qdom.setContent(&fh))
	{
		foreach (const QDomNode &item_node, getChildren(qdom.documentElement(), "ringtones/item"))
		{
			int id_ringtone = getTextChild(item_node, "id_ringtone").toInt();
			QString filename = getTextChild(item_node, "descr");
			ringtone_to_file[static_cast<Ringtones::Type>(id_ringtone)] = filename;
		}
	}
}

void RingtonesManager::playRingtone(Ringtones::Type t)
{
	return; // REMOVEME
	playSound(typeToFilePath(t));
}

void RingtonesManager::playRingtone(int ring)
{
	return; // REMOVEME
	Q_ASSERT_X(ring >= 0 && ring < ringtone_to_file.size(), "RingtonesManager::playRingtone(int)",
		"Given ringtone is outside valid range.");
	playSound(ringtone_to_file[ring]);
}

void RingtonesManager::stopRingtone()
{
	return; // REMOVEME
	stopSound();
}

void RingtonesManager::setRingtone(Ringtones::Type t, int item_id, int ring)
{
	return; // REMOVEME
	Q_ASSERT_X(ring >= 0 && ring < ringtone_to_file.size(), "RingtonesManager::setRingtone",
		"Given ringtone is outside valid range.");
	type_to_ringtone[t] = ring;

	setCfgValue("id_ringtone", ring, item_id);
}

int RingtonesManager::getRingtone(Ringtones::Type t)
{
	return 0; // REMOVEME
	return type_to_ringtone[t];
}

int RingtonesManager::getRingtonesNumber()
{
	return 1; // REMOVEME
	return ringtone_to_file.size();
}

QString RingtonesManager::typeToFilePath(Ringtones::Type t)
{
	return ringtone_to_file[type_to_ringtone[t]];
}


RingtonesManager *bt_global::ringtones = 0;
