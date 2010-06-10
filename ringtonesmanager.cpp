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
#include "mediaplayer.h"  // bt_global::sound
#include "xml_functions.h" // getChildren, getTextChild
#include "generic_functions.h" // setCfgValue

#include <QStringList>
#include <QDomDocument>
#include <QFileInfo>
#include <QDebug>
#include <QDir>



RingtonesManager::RingtonesManager(QString ringtone_file)
{
	QFile fh(ringtone_file);
	QDomDocument qdom;

	QDir dirname = QFileInfo(fh).absoluteDir();
	if (qdom.setContent(&fh))
	{
		QDomNode ring_node = getChildWithName(qdom.documentElement(), "ringtones");
		foreach (const QDomNode &item_node, getChildren(ring_node, "item"))
		{
			int id_ringtone = getTextChild(item_node, "id_ringtone").toInt();
			QString filename = getTextChild(item_node, "descr");
			ringtone_to_file[static_cast<Ringtones::Type>(id_ringtone)] =
				QFileInfo(dirname, filename).absoluteFilePath();
		}
	}
}

void RingtonesManager::playRingtone(Ringtones::Type t)
{
	if (!type_to_ringtone.contains(t))
	{
		qWarning() << "Unable to play a ringtone for type: " << t;
		return;
	}

	qDebug() << "RingtonesManager::playRingtone:" << ringtone_to_file[type_to_ringtone[t]]
		<< "for type:" << t;
	bt_global::sound->play(ringtone_to_file[type_to_ringtone[t]]);
}

void RingtonesManager::playRingtone(int ring)
{
	Q_ASSERT_X(ringtone_to_file.contains(ring), "RingtonesManager::playRingtone(int)",
		qPrintable(QString("Given ringtone %1 is outside valid range.").arg(ring)));

	bt_global::sound->play(ringtone_to_file[ring]);
}

void RingtonesManager::stopRingtone()
{
	bt_global::sound->stop();
}

void RingtonesManager::setRingtone(Ringtones::Type t, int item_id, int ring)
{
	Q_ASSERT_X(ringtone_to_file.contains(ring), "RingtonesManager::setRingtone",
		qPrintable(QString("Given ringtone %1 is outside valid range.").arg(ring)));
	type_to_ringtone[t] = ring;

	setCfgValue("id_ringtone", ring, item_id);
}

int RingtonesManager::getRingtonesNumber()
{
	return ringtone_to_file.size();
}

RingtonesManager *bt_global::ringtones = 0;
