#ifndef BANN_LIGHTING_H
#define BANN_LIGHTING_H

#include "device_status.h"
#include "bannregolaz.h"
#include "bannonoff.h"
#include "bannonoff2scr.h"
#include "bannon2scr.h"

#include <QWidget>
#include <QString>
#include <QList>

class device;
class openwebnet;

/*!
 * \class dimmer
 * \brief This is the dimmer-banner class.
 * \author Davide
 * \date lug 2005
 */
class dimmer : public bannRegolaz
{
Q_OBJECT
public:
	dimmer(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon, QString breakIcon, bool to_be_connect=true);
	virtual void inizializza(bool forza = false);
	void Draw();
protected:
	char pul;
	char gruppi[4];
	device *dev;
private slots:
	virtual void Accendi();
	virtual void Spegni();
	virtual void Aumenta();
	virtual void Diminuisci();
public slots:
	virtual void status_changed(QList<device_status*>);
signals:
	void frame_available(char *);
};


/*!
 * \class dimmer 100 livelli
 * \brief This is the 100 lev dimmer-banner class.
 * \author Ciminaghi
 * \date Mar 2006
 */
class dimmer100 : public dimmer
{
Q_OBJECT;
public:
	dimmer100(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon,
		QString inactiveIcon, QString breakIcon, int sstart, int sstop);
	void inizializza(bool forza=false);
	void status_changed(QList<device_status*>);
private:
	/*!
	 * \brief decode msg code, lev and speed from open message
	 * returns true if msg_open is a message for a new dimmer,
	 * false otherwise
	 */
	bool decCLV(openwebnet&, char& code, char& lev, char& speed, char& h, char& m, char& s);
	int softstart, softstop;
	int last_on_lev;
	int speed;
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
};


/*!
 * \class grDimmer
 * \brief This class is made to control a number of dimmers.
 *
 * It behaves essentially like \a dimmer even if it doesn't represent the dimmer's state 
 * since different dimmers can have different states.
 * \author Davide
 * \date lug 2005
 */
class grDimmer : public bannRegolaz
{
Q_OBJECT
public:
	grDimmer(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString Iconsx, QString Icondx);
	void inizializza(bool forza = false);
protected:
	QList<QString> elencoDisp;
private slots:
	virtual void Attiva();
	virtual void Disattiva();
	virtual void Aumenta();
	virtual void Diminuisci();
private:
	void sendFrame(QString msg);
};


/*!
 * \class grDimmer100
 * \brief This class is made to control a number of 100 levels dimmers
 *
 * It behaves essentially like \a dimmer group
 * \author Davide
 * \date Jun 2006
 */
class grDimmer100 : public grDimmer
{
Q_OBJECT
public:
	grDimmer100(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx,QString Iconsx,
		QString Icondx, QList<int> sstart, QList<int> sstop);
private:
	QList<int> soft_start;
	QList<int> soft_stop;
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
};


/*!
 * \class grAttuatAutom
 * \brief This class is made to control a number of automation actuators.
 *
 * It behaves essentially like attuatAutom but it doesn't represent the actuators 
 * state since different actuators can have different states.
 * \author Davide
 * \date lug 2005
 */
class grAttuatAutom : public bannOnOff
{
Q_OBJECT
private:
	QList<QString> elencoDisp;
public:
	grAttuatAutom(QWidget *parent, QList<QString> addresses, QString IconaSx=QString(), QString IconaDx=QString(), QString Icon=QString(), int periodo=0, int numFrame=0);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/
private slots:
	void Attiva();
	void Disattiva();
};


/*!
 * \class attuatAutomTemp
 * \brief This class is made to manage a timed control.
 *
 * With this object is possible to have the actuator active for a certain time. After that time the actuator releases by itself.
 * Clicking on the left button it is possible to select the time the actuator has to remain active; clicking on the right button
 * the actuation effectively starts.
 * \author Davide
 * \date lug 2005
 */
class attuatAutomTemp : public bannOnOff2scr
{
Q_OBJECT
public:
attuatAutomTemp(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(), QString IconaDx=QString(),
		QString IconActive=QString(), QString IconDisactive=QString(), int periodo=0, int numFrame=0, QList<QString> lt = QList<QString>());
public slots:
	virtual void status_changed(QList<device_status*>);
protected:
	uchar cntTempi;
	QString tempo_display;
	QList<QString> tempi;
	device *dev;
	uchar ntempi();
	QString leggi_tempo();
	virtual void assegna_tempo_display();
	virtual void inizializza(bool forza=false);
protected slots:
	virtual void Attiva();
private slots:
	void CiclaTempo();
};


/*!
 * \class attuatAutomTempN
 * \brief This class is made to manage a timed control.
 *
 * This object implements the new timed actuator with N time settings
 * With this object is possible to have the actuator active for a certain time. After that time the actuator releases by itself.
 * Clicking on the left button it is possible to select the time the actuator has to remain active; clicking on the right
 * button the actuation effectively starts.
 * \author Ciminaghi
 * \date Apr 2006
 */
class attuatAutomTempNuovoN : public attuatAutomTemp
{
Q_OBJECT
public:
	attuatAutomTempNuovoN(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(), QString IconaDx=QString(),
		QString IconActive=QString(), QString IconDisactive=QString(), int periodo=0, int numFrame=0, QList<QString> lt = QList<QString>());
public slots:
	void status_changed(QList<device_status*>);
protected:
	bool stato_noto;
	virtual void assegna_tempo_display();
	virtual void inizializza(bool forza=false);
protected slots:
	void Attiva();
};


/*!
 * \class attuatAutomTempF
 * \brief This class is made to manage a timed control.
 *
 * This object implements the new timed actuator with fixed time setting
 * \author Ciminaghi
 * \date Apr 2006
 */
class attuatAutomTempNuovoF : public bannOn2scr
{
Q_OBJECT
public:
	attuatAutomTempNuovoF(QWidget *parent=0, QString where=QString(), QString IconaCentroSx=QString(),
		QString IconaCentroDx=QString(), QString IconDx=QString(), QString t=QString());
public slots:
	void status_changed(QList<device_status*>);
protected:
	// TODO: rendere tempo una qstring!
	char tempo[20];
	void chiedi_stato();
	int h, m, s, val;
	QTimer *myTimer;
	bool stato_noto;
	bool temp_nota;
	bool update_ok;
	int  tentativi_update;
protected slots:
	void Attiva();
	void update();
private:
	device *dev;
	void inizializza(bool forza=false);
	 // This function mask the other overload of banner::SetIcons. I don't care about it.
	void SetIcons(QString i1, QString i2, QString i3);
	void Draw();
};

#endif
