#ifndef BANN_ANTINTRUSION_H
#define BANN_ANTINTRUSION_H

#include "bannonicons.h"
#include "bann2butlab.h"
#include "device_status.h"

#include <QString>
#include <QList>

/// Forward Declarations
class tastiera;
class device;


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
public:
	zonaAnti(QWidget *parent, const QString &name, QString indirizzo, QString iconzona, QString IconDisactive, QString IconActive);
	void inizializza(bool forza = false);
	void Draw();
public slots:
	void status_changed(QList<device_status*>);
	void ToggleParzializza();
	void abilitaParz(bool);
	void clearChanged(void);
	int getIndex();
private:
	void SetIcons();
	QString parzIName, sparzIName;
	QString zonaAttiva;
	QString zonaNonAttiva;
	bool already_changed;
	device *dev;
signals:
	void partChanged(zonaAnti *);
};


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
public:
	impAnti(QWidget *parent, QString IconOn, QString IconOff, QString IconInfo, QString IconActive);
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
private:
	static const int MAX_ZONE = 8;
	tastiera *tasti;
	zonaAnti *le_zone[MAX_ZONE];
	bool send_part_msg;
	bool part_msg_sent;
	bool inserting;
	char *passwd;
	device *dev;
private slots:
	void inizializza(bool forza = false);
	void Inserisci();
	void Disinserisci();
	void Insert1(char*);
	void Insert2();
	void Insert3();
	void DeInsert(char*);
signals:
	void impiantoInserito();
	void abilitaParz(bool);
	void clearChanged(void);
	void clearAlarms(void);
};

#endif
