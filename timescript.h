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


#ifndef TIMESCRIPT_H
#define TIMESCRIPT_H

#include <QLCDNumber>
#include <QDateTime>

/*!
  \class timeScript
  \brief This class shows a time and eventually a date with the possibility to modify them using a set of appropriate slots.

  The argument are the pointer to the parent Widget, the pointer to the name, the type (0=shows the local time with hh:mm:ss, 1=, 2=shows a stopped time with hh:mm, 25= shows the date), 
  \date lug 2005
*/
class timeScript : public QLCDNumber
{
    Q_OBJECT
public:
	timeScript(QWidget *parent=0, uchar type=0, QDateTime* mioOrol=NULL);
	~timeScript();

	void showDate();
	QDateTime getDataOra();
	void setDataOra(QDateTime);

protected:
	void timerEvent(QTimerEvent *);
	void mousePressEvent(QMouseEvent *);

private slots:
    void stopDate();
public slots:
/*!
  \brief Shows the time.
*/
	void showTime();

/*!
  \brief Goes back to time
*/
	void reset();

/*!
  \brief Decreases seconds value.
*/
	void diminSec();

/*!
  \brief Decreases minuts value.
*/
	void diminMin();

/*!
  \brief Decreases hours value.
*/
	void diminOra();

/*!
  \brief Decreases days value.
*/
	void diminDay();

/*!
  \brief Decreases months value.
*/
	void diminMonth();

/*!
  \brief Decreases years value.
*/
	void diminYear();

/*!
  \brief Increases seconds value.
*/
	void aumSec();

/*!
  \brief Increases minutes value.
*/
	void aumMin();

/*!
  \brief Increases hours value.
*/
	void aumOra();

/*!
  \brief Increases days value.
*/
	void aumDay();

/*!
  \brief Increases months value.
*/
	void aumMonth();

/*!
  \brief Increases years value.
*/
	void aumYear();

private:
    int normalTimer;
    int showDateTimer;
    uchar type;
    QDateTime* mioClock;
};


#endif // TIMESCRIPT_H
