
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
public slots:
	void hide();
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
	impostaSveglia(QWidget *, const char * , contdiff*, char* , char* , char*, char*, int, int, char*,int);
	/*!
	\brief changes the abilitation af the alarm set
	*/
	void setAbil(bool);
	/*!
	\brief forces a eeprom read to initialyze alarm set settings
	*/
	void inizializza();

signals:
/*!
\brief Emitted to turn alarm clock off
*/
	void spegniSveglia();
public slots:
/*!
\brief Analizes \a Open \a frames coming form the system
*/
	void gestFrame(char*);
/*!
\brief changes the alarm set abilitation
*/
	void toggleAbil();
/*!
\brief Shows the object
*/
	void show();
private:
	sveglia* svegliolina;
	char iconOn[50];
	char iconOff[50];
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
	calibration(sottoMenu  *, const char *,const char *);

private slots:
	void doCalib();
	void fineCalib();
signals:
	void startCalib();
	void endCalib();
private:
	Calibrate* calib;
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
	impBeep(sottoMenu  *, const char *,char *,const char *,const char *);
public slots:
	void toggleBeep();
private:
	char iconOn[50];
	char iconOff[50];
};


class impContr : public bannOnDx
{
Q_OBJECT
public:
	impContr(sottoMenu  *, const char *,char *,const char *);
public slots:
	void hide();

private slots:
	void showContr();
	void contrMade();
private:
	contrPage* contrasto;
};


class machVers : public bannOnDx
{
Q_OBJECT
public:
	machVers(sottoMenu  *, const char *,  versio*, const char*);
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
	impPassword (QWidget *,const char *, char* , char*, char*,char*,int);
signals:
/*!
  \brief  Emitted when the password is (dis)abilitated so BtMain knows if has to ask password or not
*/
	void activatePaswd(bool);
/*!
  \brief  Emitted when the password is changed so BtMain knows which password has to wait
*/
	void setPwd(bool,char*);
public slots:
/*!
  \brief  Changes the activation state
*/ 
	void toggleActivation();
/*!
  \brief  Shows the banner
*/ 
	void show();
/*!
  \brief Hides what has to be hidden
*/
	void hide();
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
	void setEnabled (bool);
private:
	char paswd[10];
	bool active;
	char iconOn[50];
	char iconOff[50];
	tastiera *tasti;
	bool sb;
	unsigned char starting;
};
#endif //SET_ITEMS_H



