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


#ifndef BUTTONS_BAR_H
#define BUTTONS_BAR_H

#include "btbutton.h"

#include <QButtonGroup>
#include <QPixmap>
#include <QVector>
#include <QWidget>



/** \class ButtonsBar
 *  this class implements a buttons bar
 */
class ButtonsBar : public QWidget
{
Q_OBJECT
public:
	ButtonsBar(QWidget *parent = 0, unsigned int number_of_buttons = 1, Qt::Orientation orientation = Qt::Horizontal);
	~ButtonsBar();
	bool setButtonIcon(unsigned int button_number, const QString &icon_path);
	
	void showButton(int idx);
	void hideButton(int idx);
private:
	QVector<BtButton*>  buttons_list;
	QButtonGroup *buttons_group;
signals:
	/// The signal pressed(int ) of QButton group is connected to the following by the constructor
	void clicked(int button_number);
};

#endif // BUTTONS_BAR_H
