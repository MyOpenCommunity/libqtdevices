/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** scenari.h
 **
 **definizioni dei vari scenari implementati
 **
 ****************************************************************/

#ifndef SCENARI_H
#define SCENARI_H

#include "bannondx.h" // class bannOnSx
#include "bann4taslab.h"
#include "bann3but.h"
#include "bann4but.h"
#include "device_status.h"

#include <QObject>
#include <QString>
#include <QList>

class scenEvo_cond;
class device;
class QWidget;

/*****************************************************************
 **scenario
 ****************************************************************/
/*!
 * \class scenario
 * \brief This class is made to control a scenario of a \a scenario \a unit.
 *
 * \author Davide
 * \date lug 2005
 */
class scenario : public bannOnSx
{
Q_OBJECT
public:
	scenario(sottoMenu  *, const char *, char *, char *);
	void inizializza(bool forza = false);
private slots:
	void Attiva();
private:
	device *dev;
};

/*****************************************************************
 **Gestione Modulo scenari
 ****************************************************************/
/*!
 * \class gesModScen
 * \brief This class is made to control a scenario of a \a din \a scenario \a module.
 *
 * From this banner is possible to actuate, clean and program the scenario controlled.
 * \author Davide
 * \date lug 2005
 */
class gesModScen : public  bann4tasLab
{
Q_OBJECT
private:
	char iconOn[50];
	char iconStop[50];
	char iconInfo[50];
	char iconNoInfo[50];
	char cosa[10];
	char dove[10];
	unsigned char sendInProgr, bloccato, in_progr;
	device *dev;
public:
	gesModScen(QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* Ico1=NULL,char* Ico2=NULL,char* Ico3=NULL,char* Ico4=NULL, char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL);
private slots:
	void attivaScenario();
	void enterInfo();
	void exitInfo();
	void startProgScen();
	void stopProgScen();
	void cancScen();
public slots:
	void status_changed(QList<device_status*>);
	void inizializza(bool forza = false);
};


/*****************************************************************
 ** Scenario evoluto
 ****************************************************************/
/*!
 * \class scenEvo
 * \brief This class represents an advanced scenario management object
 * \author Ciminaghi
 * \date apr 2006
 */
class scenEvo : public  bann3But
{
Q_OBJECT
private:
	QList<scenEvo_cond*> condList;
	unsigned current_condition;
	QString action;
	int serial_number;
	static int next_serial_number;

public:
	scenEvo(QWidget *parent=0, const char *name=NULL, QList<scenEvo_cond*> *c=NULL, char* Ico1=NULL,char* Ico2=NULL,char* Ico3 = NULL,char* Ico4=NULL, char* Ico5=NULL, char* Ico6=NULL, char* Ico7=NULL, QString act="", int enabled = 0);
	~scenEvo();
	void Draw();
private slots:
	void toggleAttivaScev();
	void configScev();
	void forzaScev();
	void nextCond();
	void prevCond();
	void firstCond();
	void trig(bool forced = false);
	void freezed(bool);
	void saveAndApplyAll();
	void resetAll();
	void hide();
	void trigOnStatusChanged();
public slots:
	void inizializza(bool forza = false);
};


/*****************************************************************
 ** Scenario schedulato
 ****************************************************************/
/*!
 * \class scenSched
 * \brief This class represents a scheduled scenario management object
 * \author Ciminaghi
 * \date apr 2006
 */
class scenSched : public  bann4But
{
Q_OBJECT
private:
	QString action_enable;
	QString action_disable;
	QString action_start;
	QString action_stop;
public:
	scenSched(QWidget *parent=0, const char *name=NULL, char* IconaSx="",char *IconaCSx="", char *IconaCDx="", char* IconaDx="", char *action_enable="", char *action_disable="", char *action_start="", char *action_stop="");
	/*!
	 * Reimplemented draw
	 */
	void Draw();
public slots:
	/*!
	 * \brief Enable scheduled scenario
	 */
	void enable();
	/*! Disable scheduled scenario */
	void disable();
	/*! Start scheduled scenario */
	void start();
	/*! Stop scheduled scenario */
	void stop();
};

#endif
