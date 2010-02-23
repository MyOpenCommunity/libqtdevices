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

#include "page.h"

#include <QString>

class QLabel;

/*!
  \class Version
  \brief This class shows the device versions.

  It shows \a software versione, \a pic version and \a hardware version and the seial number of the device among all the TouchScreens installed in the system.
  Software version is a serial number considering kernel, stack open and MMI. Pic version is the release of the SCS-interface micro-controller. Hw version is the PCB release (0=T7777A, 1=T7777B and so on). The serial number of the device is necessary to make diagnostic operation possible (making it watchable in this page facilitates configuration errors discovery)
  \date lug 2005
*/  
class Version : public Page
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

public slots:
	/*!
	\brief Analyze the \a Open \a frame incoming to verify the versions to be shown
	*/
	void gestFrame(char*);
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
