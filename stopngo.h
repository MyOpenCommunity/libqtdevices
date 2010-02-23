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


#ifndef STOPNGO_H
#define STOPNGO_H

#include "page.h"
#include "bann1_button.h"
#include "btbutton.h"


#include <QList>

class device_status;
class QLCDNumber;
class QWidget;
class QTimer;
class QLabel;


/*!
  \class StopngoItem
  \brief Class defining one item of device Stop&Go.

  \author Lucio Macellari - Mac S.r.l.
  \date May 2008
  */
class StopngoItem
{
public:
	StopngoItem(int _id=0, int _cid=0, QString _descr="", QString _where="0");

	void SetId(int id);
	void SetCid(int cid);
	void SetDescr(QString descr);
	void SetWhere(QString where);

	int GetId();
	int GetCid();
	QString GetDescr();
	QString GetWhere();

private:
	int id;
	int cid;
	QString descr;
	QString where;
};


/*!
  \class BannPulsDynIcon
  \brief Class derived from bannPuls with status changed event handling.

  \author Lucio Macellari - Mac S.r.l.
  \date May 2008
*/
class BannPulsDynIcon : public bannPuls
{
Q_OBJECT
public:
	BannPulsDynIcon(QWidget *parent);
	~BannPulsDynIcon();

public slots:
	void status_changed(QList<device_status*>);
};



/*!
  \class StopngoPage
  \brief Stop & go devices GUI page.
  
  \author Lucio Macellari - Mac S.r.l.
  \date jul 2008
*/
class StopngoPage : public Page
{
Q_OBJECT
public:
	StopngoPage(QString where, int id = 0, QString pageTitle = "");
	~StopngoPage();

private:
	QString pageTitle, where;
	int id, statusBmp, autotestFreq;
	QLabel *titleBar, *labelAutoArm, *labelVerify;
	BtButton *okBut, *autoArmBut, *onBut, *offBut, *verifyBut, *autotestBut, *minusBut, *plusBut;
	QLabel *statusIcon;
	QLCDNumber *freqLcdNumber;
	QTimer *freqSendTimer;

	void AddItems();
	void SetStatusIcon(const char *);
	void SetButtonIcon(BtButton *, const char *);
	void SetFreqValue(int);

public slots:
	void status_changed(QList<device_status*>);

private slots:
	void AutoArmClick();
	void OnClick();
	void OffClick();
	void VerifyClick();
	void AutotestClick();
	void MinusClick();
	void PlusClick();
	void LoadTimer();
	void FireFreqFrame();
};

#endif
