/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** actuators.h
 **
 **definizioni dei vari attuatori implementati
 **
 ****************************************************************/

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "device_status.h" // device, device_status
#include "bannonoff.h"
#include "bannonoff2scr.h"
#include "bannon2scr.h"
#include "bann3but.h"
#include "bannpuls.h"
#include "main.h" // MAX_PATH

class device;


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

public:
	attuatAutom(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive, QString IconDisactive, int periodo=0, int numFrame=0);
public slots:
	void status_changed(QList<device_status*>);
private:
	char gruppi[4];
	device *dev;
	void inizializza(bool forza=false);
private slots:
	void Attiva();
	void Disattiva();
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
	// TODO: probabilmente sarebbe meglio che diventasse QList<QString> ma viene creata
	// nell'xmlconfhandler, e passata come void*.
	QList<QString*> elencoDisp;
public:
	grAttuatAutom(QWidget *parent=0, void *indirizzi=NULL, QString IconaSx=QString(), QString IconaDx=QString(), QString Icon=QString(), int periodo=0, int numFrame=0);
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
	// TODO: la lista QList<QString*> *lt contiene stringhe che vengono create esternamente (dall'xmlconfhandler?) e distrutte
	// dal distruttore della classe.. decidere di chi deve essere l'ownership di questi oggetti!!
	attuatAutomTemp(QWidget *parent=0, char *indirizzo=NULL, QString IconaSx=QString(), QString IconaDx=QString(),
		QString IconActive=QString(), QString IconDisactive=QString(), int periodo=0, int numFrame=0, QList<QString*> *lt = NULL);
	~attuatAutomTemp();
public slots:
	virtual void status_changed(QList<device_status*>);
protected:
	uchar cntTempi;
	QString tempo_display;
	QList<QString*> tempi;
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
	attuatAutomTempNuovoN(QWidget *parent=0, char *indirizzo=NULL, QString IconaSx=QString(), QString IconaDx=QString(),
		QString IconActive=QString(), QString IconDisactive=QString(), int periodo=0, int numFrame=0, QList<QString*> *lt = NULL);
public slots:
	void status_changed(QList<device_status*>);
protected:
	bool stato_noto;
	virtual void assegna_tempo_display();
	virtual void inizializza(bool forza=false);
protected slots:
	void Attiva();
};


/*****************************************************************
 **attuatore automazione temporizzato nuovo a tempo fisso
 ****************************************************************/
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
	attuatAutomTempNuovoF(QWidget *parent=0, char *indirizzo=NULL, QString IconaCentroSx=QString(),
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

/*!
 * \class attuatPuls
 * \brief This class is made to manage a pulse automation.
 *
 * This object is useful when there's the need to activate something only during the pressure time 
 * (for instance when dealing with a lock).
 * \author Davide
 * \date lug 2005
 */
class attuatPuls : public bannPuls
{
Q_OBJECT
public:
	attuatPuls(QWidget *parent=0, QString where=QString(), QString IconaSx=QString(), QString IconActive=QString(),
		char tipo=0, int periodo=0, int numFrame=0);
private slots:
	void Attiva();
	void Disattiva();
private:
	char type;
	void inizializza(bool forza = false);
};

#endif
