
/*!
 * \file
 * <!--
 * Copyright 2008 MAC S.r.l. (http://www.mac-italia.com/)
 * All rights reserved.
 * -->
 *
 * \brief  Stop & go related classes
 *
 *  TODO: detailed description (optional) 
 *
 * \author Lucio Macellari
 */

#ifndef STOPNGO_H
#define STOPNGO_H

#include "bannpuls.h"
#include "btbutton.h"

#include <QWidget>
#include <QList>

class device_status;
class QLCDNumber;
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
class StopngoPage : public QWidget
{
Q_OBJECT
public:
	StopngoPage(QWidget *parent, QString where, int id = 0, QString pageTitle = "");
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
	void showPage();

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

signals:
	void Closed();
};

#endif
