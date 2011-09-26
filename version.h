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


#ifndef VERSION_H
#define VERSION_H

#include "frame_classes.h" // FrameReceiver
#include "page.h"

#include <QString>

class QLabel;

// Shows the touchscreen version.
class Version : public Page, public FrameReceiver
{
Q_OBJECT
public:
	Version();
	/*!
	\brief Initialize the page asking the versions to the open stack.
	*/
	virtual void inizializza();
	/*!
	\brief Sets the serial number of the device among all the TouchScreens installed in the system
	*/
	void setAddr(int);
	/*!
	\brief Sets model string
	*/
	void setModel(const QString &);

	virtual void manageFrame(OpenMsg &msg);

signals:
	void exitRequested();

protected:
	virtual void mouseReleaseEvent(QMouseEvent *);

private:
	unsigned char vers;
	unsigned char release;
	unsigned char build;
	unsigned char pic_version;
	unsigned char pic_release;
	unsigned char pic_build;
	unsigned char hw_version;
	unsigned char hw_release;
	unsigned char hw_build;
	QString model;
	QLabel *box_text;
	int indDisp;
};

#endif //VERSION_H
