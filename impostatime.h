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


#ifndef IMPOSTA_TIME_H
#define IMPOSTA_TIME_H

#include "page.h"

class timeScript;
class BtButton;
class QLabel;


/*!
  \class impostaTime
  \brief This is the class used to set time and date.

  \author Davide
  \date lug 2005
*/

class  impostaTime : public Page
{
Q_OBJECT
public:
	impostaTime();
	BtButton *but[7];

private slots:
	void OKTime();
	void OKDate();

private:
	timeScript *dataOra;
	QLabel *Immagine;
	void setDatePage();

private slots:
	void setTimePage();

};


#endif // IMPOSTA_TIME_H
