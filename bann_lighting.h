#ifndef BANN_LIGHTING_H
#define BANN_LIGHTING_H

#include "bannregolaz.h"
#include "bann2_buttons.h" // bannOnOff, bannOnOff2scr
#include "bann1_button.h" // bannOn2scr
#include "lighting_device.h" // BasicTime

#include <QWidget>
#include <QString>
#include <QList>


class device;
class device_status;
class openwebnet;
class LightingDevice;
class QDomNode;
class DimmerDevice;
class Dimmer100Device;


class LightGroup : public BannOnOffNew
{
Q_OBJECT
public:
	LightGroup(QWidget *parent, const QDomNode &config_node, const QList<QString> &addresses);
	// TODO: do we need a inizializza() method? The original class didn't have it...

private slots:
	void lightOn();
	void lightOff();

private:
	// of course these pointers must not be deleted since objects are owned by
	// the cache
	QList<LightingDevice *> devices;
};

class DimmerBase : public bannRegolaz
{
Q_OBJECT
public:
	DimmerBase(QWidget *parent);
	virtual void Draw();
};

// TODO: to be renamed when dimmer is gone
class DimmerNew : public DimmerBase
{
Q_OBJECT
public:
	DimmerNew(QWidget *parent, const QDomNode &config_node, QString where);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();
	void status_changed(const StatusList &sl);

private:
	DimmerDevice *dev;
	int light_value;
};

class DimmerGroup : public bannRegolaz
{
Q_OBJECT
public:
	DimmerGroup(QWidget *parent, const QDomNode &config_node, QList<QString> addresses);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	QList<DimmerDevice *> devices;
};

// TODO: to be renamed when dimmer100 is gone
class Dimmer100New : public DimmerBase
{
Q_OBJECT
public:
	Dimmer100New(QWidget *parent, const QDomNode &config_node);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();
	void status_changed(const StatusList &sl);

private:
	Dimmer100Device *dev;
	int start_speed, stop_speed;
	int light_value;
};

class Dimmer100Group : public bannRegolaz
{
Q_OBJECT
public:
	Dimmer100Group(QWidget *parent, const QDomNode &config_node);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	QList<Dimmer100Device *> devices;
	QList<int> start_speed, stop_speed;
};


class TempLight : public bannOnOff2scr
{
Q_OBJECT
public:
	TempLight(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza);

protected:
	virtual void readTimes(const QDomNode &node);
	void updateTimeLabel();

	QList<struct BasicTime> times;
	int time_index;
	LightingDevice *dev;

protected slots:
	virtual void activate();

private slots:
	void status_changed(const StatusList &sl);
	void cycleTime();
};

class TempLightVariable : public TempLight
{
Q_OBJECT
public:
	TempLightVariable(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza);

protected:
	virtual void readTimes(const QDomNode &node);

protected slots:
	virtual void activate();
};


class TempLightFixed : public bannOn2scr
{
Q_OBJECT
public:
	TempLightFixed(QWidget *parent, const QDomNode &config_node);
	virtual void Draw();
	virtual void inizializza(bool forza);

private:
	void SetIcons(QString on_icon, QString status_icon, QString time_icon);
	LightingDevice *dev;
};

#if 1
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
protected:
	QList<QString> elencoDisp;
private:
	void sendMsg(QString msg);
private slots:
	virtual void Attiva();
	virtual void Disattiva();
	virtual void Aumenta();
	virtual void Diminuisci();
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
attuatAutomTemp(QWidget *parent, QString where, QString IconaSx, QString IconaDx,
		QString IconActive, QString IconDisactive, QList<QString> lt);
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
	attuatAutomTempNuovoN(QWidget *parent, QString where, QString IconaSx, QString IconaDx,
		QString IconActive, QString IconDisactive, QList<QString> lt);
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
	attuatAutomTempNuovoF(QWidget *parent, QString where, QString IconaCentroSx,
		QString IconaCentroDx, QString IconDx, QString t);
public slots:
	void status_changed(QList<device_status*>);
protected:
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
	QString tempo;
	void inizializza(bool forza=false);
	 // This function mask the other overload of banner::SetIcons. I don't care about it.
	void SetIcons(QString i1, QString i2, QString i3);
	void Draw();
};
#endif
#endif
