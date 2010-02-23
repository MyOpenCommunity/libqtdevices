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


#ifndef SPECIALPAGE_H
#define SPECIALPAGE_H

#include "page.h"

class timeScript;
class TemperatureViewer;

class QDomNode;


class SpecialPage : public Page
{
Q_OBJECT
public:
	SpecialPage(const QDomNode &config_node);
	virtual void inizializza();

private:
	void loadItems(const QDomNode &config_node);
	void loadSpecial(const QDomNode &config_node);

	// Define the behaviour of special button
	enum specialType
	{
		NORMAL = 0,  /*!<  Clicking the \a special \a button the device always make the same function.*/
		CYCLIC = 1,  /*!< Clicking the \a special \a button the device one time make a \a ON command and then an \a OFF command an so on.*/
		BUTTON = 2   /*!<  Pushing the \a special \a button the device make a \a ON command while Releasing the button a \a OFF command is made.*/
	};

	specialType type; // The type of special button
	QString who, what, where;
	TemperatureViewer *temp_viewer;

private slots:
	void clickedButton();
	void pressedButton();
	void releasedButton();
	void gestFrame(char *frame);
};

#endif // SPECIALPAGE_H
