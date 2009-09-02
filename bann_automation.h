#ifndef BANN_AUTOMATION_H
#define BANN_AUTOMATION_H

#include "bann1_button.h" // bannPuls
#include "bann3_buttons.h" // bann3But
#include "bann2_buttons.h" // bannOnOff

#include <QWidget>
#include <QString>

/// Forward Declarations
class device;
class device_status;
class PPTStatDevice;

/*!
 * \class
 * \brief This class represents an automated video-doorphone actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatVC : public bannPuls
{
Q_OBJECT
public:
	automCancAttuatVC(QWidget *parent, QString where, QString IconaSx, QString IconaDx);
private:
	device *dev;
private slots:
	void Attiva();
};


/*!
 * \class
 * \brief This class represents an automated light actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatIll : public bannPuls
{
Q_OBJECT
public:
	automCancAttuatIll(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString t);
private:
	device *dev;
	QString time;
private slots:
	void Attiva();
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
	attuatAutomIntSic(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive, QString IconDisactive);
	virtual void inizializza(bool forza = false);
public slots:
	virtual void status_changed(QList<device_status*>);
private:
	char uprunning ,dorunning;
	QString icon_sx, icon_dx, icon_stop;
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
	attuatAutomInt(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive, QString IconDisactive);
	virtual void inizializza(bool forza = false);
public slots:
	virtual void status_changed(QList<device_status*>);
private slots:
	void analizzaUp();
	void analizzaDown();
private:
	char uprunning,dorunning;
	QString icon_sx, icon_dx, icon_stop;
	device *dev;
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
	grAttuatInt(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString Icon);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
public slots:
	void Alza();
	void Abbassa();
	void Ferma();
private:
	QList<QString> elencoDisp;
	void sendAllFrames(QString msg);
};


class PPTStat : public banner
{
Q_OBJECT
public:
	PPTStat(QWidget *parent, QString where, int cid);
	virtual void inizializza(bool forza=false);

private:
	PPTStatDevice *dev;
	QString img_open, img_close;
private slots:
	void status_changed(const StatusList &status_list);
};

#endif
