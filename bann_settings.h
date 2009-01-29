
/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann_settings.h
**
** definizioni dei vari items implementati per impostazioni
**
****************************************************************/

#ifndef BANN_SETTINGS_H
#define BANN_SETTINGS_H

#include "banner.h"
#include "bann1_button.h" // bannOnDx, bannOnSx
#include "bann2_buttons.h" // bann2But

#include <QWidget>

class Version;
class AlarmClock;
class Keypad;
class Calibrate;
class Contrast;
class contdiff;


/*!
  \class bannAlarmClock
  \brief This class is made to make alarm clock settings.

  \author Davide
  \date lug 2005
*/
class bannAlarmClock : public bann2But
{
Q_OBJECT
public:
	bannAlarmClock(QWidget *parent, contdiff *diso, int hour, int minute, QString icon1,
		QString icon2, QString icon3, int enabled, int freq, int tipo);
	/*!
	\brief changes the abilitation af the alarm set
	*/
	void setAbil(bool);
	/*!
	\brief forces a eeprom read to initialyze alarm set settings
	*/
	void inizializza();

public slots:
/*!
\brief Analizes \a Open \a frames coming form the system
*/
	void gestFrame(char*);
/*!
\brief changes the alarm set abilitation
*/
	void toggleAbil();

private:
	AlarmClock *alarm_clock;

signals:
/*!
\brief Emitted to turn alarm clock off
*/
	void spegniSveglia();
};


/*!
  \class calibration
  \brief Calibrate the device

  \author Davide
  \date lug 2005
*/
// TODO: rimuovere questa classe, e usare direttamente bannOnDx! E' necessario intervenire su Calibrate
// per modificare la gestione del grabMouse e del backup della vecchia calibrazione.
class calibration : public bannOnDx
{
Q_OBJECT
public:
	calibration(sottoMenu *parent, QString icon);

private slots:
	void doCalib();
	void fineCalib();
private:
	Calibrate* calib;
signals:
	void startCalib();
	void endCalib();
};


/*!
  \class setDataOra
  \brief Beep (dis)abilitation

  \author Davide
  \date lug 2005
*/
class impBeep : public bannOnSx
{
Q_OBJECT
public:
	impBeep(sottoMenu *parent, QString val, QString icon1, QString icon2);
public slots:
	void toggleBeep();
};


class bannContrast : public bannOnDx
{
Q_OBJECT
public:
	bannContrast(sottoMenu *parent, QString val, QString icon);

private slots:
	void done();
};


class bannVersion : public bannOnDx
{
Q_OBJECT
public:
	bannVersion(sottoMenu *parent, QString icon, Version *ver);
private slots:
	void tiempout();
	void showVers();
private:
	Version *v;
};


/*!
  \class impPassword
  \brief Manages the password.

  It's possible (dis)abilitate the password and to change the actual password.
  \author Davide
  \date lug 2005
*/
class impPassword : public bann2But
{
Q_OBJECT
public:
	impPassword(QWidget *parent, QString icon1, QString icon2, QString icon3, QString pwd, int attiva);
public slots:
/*!
  \brief  Changes the activation state
*/ 
	void toggleActivation();
/*!
  \brief  Shows the keypad to compose the password key
*/
	void reShow1();
/*!
  \brief  Shows the keypad to compose the confirmation of the password key
*/
	void reShow2();
/*!
  \brief  Stops the error beep made when the password insertion is wrong
*/
	void tiempout();

protected:
	virtual void hideEvent(QHideEvent *event);
	virtual void showEvent(QShowEvent *event);

private:
	bool active;
	QString password;
	Keypad *tasti;
	bool sb;
signals:
/*!
  \brief  Emitted when the password is (dis)abilitated so BtMain knows if has to ask password or not
*/
	void activatePaswd(bool);
};

#endif
