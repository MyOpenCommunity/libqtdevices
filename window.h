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


#ifndef WINDOW_H
#define WINDOW_H

#include "styledwidget.h"

class WindowContainer;


class Window : public StyledWidget
{
Q_OBJECT
public:
	Window();

	static void setWindowContainer(WindowContainer *container);

public slots:
	virtual void showWindow();

signals:
	void Closed();

private:
	static WindowContainer *window_container;
};

#endif // WINDOW_H

