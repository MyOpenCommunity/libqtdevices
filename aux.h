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


#ifndef AUX_H
#define AUX_H

#include "page.h"

class QLabel;


/*!
  \class Aux
  \brief This class implements the management of the aux source page

  \date lug 2006
  */
class Aux : public Page
{
Q_OBJECT
public:
	Aux(const QString &name, const QString &amb);
	void setAmbDescr(const QString &);

signals:
	/*!
	\brief Emitted when fwd button is pressed
	*/
	void Btnfwd();

private:
	QLabel *amb_descr;
};


#endif // AUX_H
