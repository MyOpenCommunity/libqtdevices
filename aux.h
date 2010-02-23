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

#include <QWidget>
#include <QString>

class QLabel;
class BtButton;
class bannFrecce;

/*!
  \class aux
  \brief This class implements the management of the aux source page

  \date lug 2006
  */
class  aux : public QWidget
{
Q_OBJECT
public:
	aux(QWidget *parent=0, const QString & name="", const QString & amb="");

	QLabel *auxName, *ambDescr;
	BtButton *fwdBut;

	/*!
	\brief Sets amb. description
	*/
	void setAmbDescr(const QString &);

	/*!
	\brief Draws the page
	*/
	void draw();

signals:
	/*!
	\brief Emitted when the page is going to be closed
	*/
	void Closed();

	/*!
	\brief Emitted when fwd button is pressed
	*/
	void Btnfwd();

public slots:
	/*!
	\brief Shows the aux details page
	*/
	void showAux();

private:
	char amb[80];
	char nome[15];
	bannFrecce * bannNavigazione;

};


#endif // AUX_H
