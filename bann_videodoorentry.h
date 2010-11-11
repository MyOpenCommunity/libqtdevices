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


#ifndef BANN_VIDEODOORENTRY_H
#define BANN_VIDEODOORENTRY_H

#include "bann2_buttons.h"

#include <QString>


/*!
	\ingroup VideoDoorEntry
	\brief Controls an entrance panel.

	Allows the user to open/close the door lock or to switch on/off the stair
	light of an entrance panel.
*/
class EntrancePanel : public Bann2Buttons
{
Q_OBJECT
public:
	EntrancePanel(QString descr, QString where, bool light, bool key);

signals:
	void stairLightActivate(QString where);
	void stairLightRelease(QString where);
	void openLock(QString where);
	void releaseLock(QString where);

private slots:
	void rightPressed();
	void rightReleased();
	void leftPressed();
	void leftReleased();

private:
	QString where;
};


#endif
