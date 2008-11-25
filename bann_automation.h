#ifndef BANN_AUTOMATION_H
#define BANN_AUTOMATION_H

#include "bannbuticon.h"
#include "bann3but.h"
#include "bannonoff.h"

#include <QWidget>
#include <QString>

/// Forward Declarations
class device;
class device_status;


/*!
 * \class
 * \brief This class represents an automated video-doorphone actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatVC : public bannButIcon
{
Q_OBJECT
private:
	device *dev;
public:
	automCancAttuatVC(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(),QString IconaDx=QString());
private slots:
	void Attiva();
};


/*!
 * \class
 * \brief This class represents an automated light actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatIll : public bannButIcon
{
Q_OBJECT
private:
	device *dev;
	QString time;
public:
	automCancAttuatIll(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(),
		QString IconaDx=QString(), QString t = QString());
private slots:
	void Attiva();
};


/*!
 * \class attuatAutom
 * \brief This is the \a automation \a actuator-banner class.
 * This class is used to represent both lightings and various kind of automation such as fan and irrigation.
 * \author Davide
 * \date lug 2005
 */
class attuatAutom : public bannOnOff
{
Q_OBJECT
private:
	char gruppi[4];
	device *dev;
	void inizializza(bool forza=false);
public:
	attuatAutom(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(), QString IconaDx=QString(), QString IconActive=QString(), QString IconDisactive=QString(),int periodo=0,int numFrame=0);
public slots:
	void status_changed(QList<device_status*>);
private slots:
	void Attiva();
	void Disattiva();
};


/*!
 * \class attuatAutomIntSic
 * \brief This class is made to manage a secure interblocked actuator.
 *
 * This kind of actuators controls gates, stretches and so on. Pushing on the right button the object closes while releasing
 * the same button the automation stops. On the left-button there's the same behavior with the difference that the object
 * is opened.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomIntSic : public bannOnOff
{
Q_OBJECT
public:
	attuatAutomIntSic(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(), QString IconaDx=QString(),
		QString IconActive=QString(), QString IconDisactive=QString());
public slots:
	virtual void status_changed(QList<device_status*>);
private:
	char uprunning ,dorunning;
	QString icon_sx, icon_dx, icon_stop;
	void inizializza(bool forza = false);
	device *dev;
private slots:
	void doPres();
	void upPres();
	void doRil();
	void upRil();
	void sendStop();
};


/*!
 * \class attuatAutomInt
 * \brief This class is made to manage an interblocked actuator.
 *
 * This kind of actuators controls gates, stretches and so on. Clicking the right button the object closes, and on the button
 * appears a stop image. A new pressure cause the stop af the automation. On the left-button there's the same behavior with
 * the difference that the object is opened.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomInt : public bannOnOff
{
Q_OBJECT
public:
	attuatAutomInt(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(), QString IconaDx=QString(), QString IconActive=QString(),
		QString IconDisactive=QString());
public slots:
	virtual void status_changed(QList<device_status*>);
private slots:
	void analizzaUp();
	void analizzaDown();
private:
	char uprunning,dorunning;
	QString icon_sx, icon_dx, icon_stop;
	device *dev;
	void inizializza(bool forza = false);
};




/*!
 * \class grAttuatInt
 * \brief This class is made to manage a number of  interblocked actuators.
 *
 * Clicking the right button the objects close,  clicking the left-button the objects open while clicking the middle 
 * button the objects stop.
 * \author Davide
 * \date lug 2005
 */
class grAttuatInt : public bann3But
{
Q_OBJECT
public:
	grAttuatInt(QWidget *parent=0, QList<QString> addresses=QList<QString>(), QString IconaSx=QString(), QString IconaDx=QString(),
		QString Icon=QString(), int periodo=0, int numFrame=0);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
public slots:
	void Alza();
	void Abbassa();
	void Ferma();
private:
	QList<QString> elencoDisp;
	void inizializza(bool forza = false);
	void sendFrame(QString msg);
};


#endif
