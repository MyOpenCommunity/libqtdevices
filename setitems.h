
/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** setitems.h
**
**definizioni dei vari items implementati per impostazioni
**
****************************************************************/

#ifndef SET_ITEMS_H
#define SET_ITEMS_H

#include "banner.h"
#include "bannondx.h"
#include "bann2but.h"
#include "impostatime.h"

#include <QWidget>

class versio;
class sveglia;
class tastiera;
class Calibrate;
class contrPage;
class contdiff;

/*!
  \class setDataOra
  \brief This class is made to set time and date.

  \author Davide
  \date lug 2005
*/
class setDataOra : public bannOnDx 
{
Q_OBJECT
public:
	setDataOra(sottoMenu *, const char *);
protected:
	virtual void hideEvent(QHideEvent *event);
private:
	impostaTime settalora;
};


/*!
  \class impostaSveglia
  \brief This class is made to make alarm clock settings.

  \author Davide
  \date lug 2005
*/
class impostaSveglia : public bann2But
{
Q_OBJECT
public:
	impostaSveglia(QWidget *parent, const char *name, contdiff *diso, QString hour, QString minute, QString icon1,
		QString icon2, int enabled, int freq, QString frame, int tipo);
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
	sveglia* svegliolina;
	QString icon_on, icon_off;

private slots:
	void forceDraw();

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
class calibration : public bannOnDx
{
Q_OBJECT
public:
	calibration(sottoMenu *parent, const char *name, QString icon);

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
	impBeep(sottoMenu *parent, const char *name, QString val, QString icon1, QString icon2);
public slots:
	void toggleBeep();
private:
	QString icon_on, icon_off;
};


class impContr : public bannOnDx
{
Q_OBJECT
public:
	impContr(sottoMenu *parent, const char *name, QString val, QString icon1);
protected:
	virtual void hideEvent(QHideEvent *event);

private slots:
	void showContr();
	void contrMade();
private:
	contrPage *contrasto;
};


class machVers : public bannOnDx
{
Q_OBJECT
public:
	machVers(sottoMenu *parent, const char *name, versio *ver, QString icon1);
private slots:
	void tiempout();
	void showVers();
private:
	versio *v;
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
	impPassword(QWidget *parent, const char *name, QString icon1, QString icon2, QString icon3, QString pwd, int attiva);
public slots:
/*!
  \brief  Changes the activation state
*/ 
	void toggleActivation();
/*!
  \brief  Shows the keypad to compose the password key
*/
	void reShow1(char*);
/*!
  \brief  Shows the keypad to compose the confirmation of the password key
*/
	void reShow2(char*);
/*!
  \brief  Stops the error beep made when the password insertion is wrong
*/
	void tiempout();
/*!
  \brief  Reimplements QWidget::setEnabled(bool)
*/
	void setEnabled(bool);

protected:
	virtual void hideEvent(QHideEvent *event);
	virtual void showEvent(QShowEvent *event);

private:
	bool active;
	QString icon_on, icon_off;
	QString password;
	tastiera *tasti;
	bool sb;
	unsigned char starting;
signals:
/*!
  \brief  Emitted when the password is (dis)abilitated so BtMain knows if has to ask password or not
*/
	void activatePaswd(bool);
/*!
  \brief  Emitted when the password is changed so BtMain knows which password has to wait
*/
	void setPwd(bool, QString);
};
#endif //SET_ITEMS_H

#ifndef BANN_CLEANSCREEN
#define BANN_CLEANSCREEN

class CleanScreen;


class BannCleanScreen : public bannOnDx
{
Q_OBJECT
public:
	BannCleanScreen(sottoMenu *parent, const char *name);
	~BannCleanScreen();
protected:
	virtual void hideEvent(QHideEvent *event);
private:
	CleanScreen *page;
};

#endif

#ifndef BANN_BRIGHTNESS
#define BANN_BRIGHTNESS

class BrightnessPage;

class BannBrightness : public bannOnDx
{
Q_OBJECT
public:
	BannBrightness(sottoMenu *parent, const char *name);
	~BannBrightness();
protected:
	virtual void hideEvent(QHideEvent *event);
private:
	BrightnessPage *page;
};
#endif //BANN_BRIGHTNESS
