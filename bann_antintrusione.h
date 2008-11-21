/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** bann_antintrusione.cpp
 **
 **
 **definizioni dei vari banner antintrusione implementati
 ****************************************************************/

#ifndef BANN_ANTINTRUSIONE_H
#define BANN_ANTINTRUSIONE_H

#include "bannonicons.h"
#include "bann2butlab.h"
#include "device_status.h"

#include <QString>
#include <QList>

/// Forward Declarations
class tastiera;
class device;


/*****************************************************************
 **Zona Antiintrusione
 ****************************************************************/
/*!
 *  \class zonaAnti
 * \brief This class is made to manage an anti-intrusion zone.
 *
 * By this banner is possible to see if the zone is active or not in a certain moment.
 * \author Davide
 * \date lug 2005
 */
class zonaAnti : public bannOnIcons
{
using banner::SetIcons; // import the set of overload of SetIcons from banner
Q_OBJECT
private:
	void SetIcons();
	QString parzIName, sparzIName;
	QString zonaAttiva;
	QString zonaNonAttiva;
	bool already_changed;
	device *dev;
public:
	zonaAnti(QWidget *parent=0, const QString & name=NULL ,char*indirizzo=NULL, QString iconzona = QString(), QString IconDisactive = QString(), QString IconActive = QString());
	void inizializza(bool forza = false);
	void Draw();
public slots:
	void status_changed(QList<device_status*>);
	void ToggleParzializza();
	void abilitaParz(bool);
	void clearChanged(void);
	int getIndex();
signals:
	void partChanged(zonaAnti *);
};


/*****************************************************************
 **Impianto Antiintrusione
 ****************************************************************/
/*!
 * \class impAnti
 * \brief This class is made to manage the anti-intrusion system.
 *
 * By this banner is possible to change the (dis)activation state of the system from the visualized one.
 * If there is an alarm queue it also possible to acces a page describing the it.
 * \author Davide
 * \date lug 2005
 */
class impAnti : public  bann2butLab
{
Q_OBJECT
private:
	static const int MAX_ZONE = 8;
	tastiera* tasti;
	zonaAnti *le_zone[MAX_ZONE];
	bool send_part_msg;
	bool part_msg_sent;
	bool inserting;
	char *passwd;
	device *dev;

public:
	impAnti(QWidget *parent=0, char *indirizzo=NULL, QString IconOn = QString(), QString IconOff = QString(), QString IconInfo = QString(), QString IconActive = QString());
private slots:
	void inizializza(bool forza = false);
	void Inserisci();
	void Disinserisci();
	void Insert1(char*);
	void Insert2();
	void Insert3();
	void DeInsert(char*);
public slots:
	void status_changed(QList<device_status*>);
	void partChanged(zonaAnti*);
	void setZona(zonaAnti*);
	int getIsActive(int zona);
	void ToSendParz(bool s);
	void openAckRx();
	void openNakRx();
protected:
	virtual void hideEvent(QHideEvent *event);
signals:
	void impiantoInserito();
	void abilitaParz(bool);
	void clearChanged(void);
	void clearAlarms(void);
};

#endif
