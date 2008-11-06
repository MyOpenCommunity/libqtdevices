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

#include "device.h"
#include "bannpuls.h"
#include "btbutton.h"

#include <QWidget>
#include <QLabel>
#include <QList>
#include <QFile>

class QLCDNumber;
class QTimer;

/*!
  \class StopngoItem
  \brief Class defining one item of device Stop&Go.

  \author Lucio Macellari - Mac S.r.l.
  \date May 2008
  */
class StopngoItem
{
public:
	StopngoItem(int id, int cid, QString descr, QString where);
	StopngoItem();
	~StopngoItem();

	inline void SetId(int id){ this->id = id; };
	inline void SetCid(int cid){ this->cid = cid; };
	inline void SetDescr(QString descr){ this->descr = descr; };
	inline void SetWhere(QString where){ this->where = where; };

	inline int GetId(){ return this->id; };
	inline int GetCid(){ return this->cid; };
	inline QString GetDescr(){ return this->descr; };
	inline QString GetWhere(){ return this->where; };
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
	BannPulsDynIcon(QWidget *, const char *);
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
	StopngoPage(QWidget *parent, const char *name, QString where, int id = 0, QString pageTitle = "");
	~StopngoPage();

protected:
	QString pageTitle, where;
	int id, statusBmp, autotestFreq;
	QLabel *titleBar, *labelAutoArm, *labelVerify;
	BtButton *okBut, *autoArmBut, *onBut, *offBut, *verifyBut, *autotestBut, *minusBut, *plusBut;
	QLabel *statusIcon;
	QLCDNumber *freqLcdNumber;
	QTimer *freqSendTimer;
	
	QString getPressedIconName(const char *);
	void AddItems();
	void SetStatusIcon(const char *);
	void SetButtonIcon(BtButton *, const char *);
	void SetFreqValue(int);

public slots:
	void status_changed(QList<device_status*>);
	void showPage();

protected slots:
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
	void sendFrame(char*);
};

#endif
