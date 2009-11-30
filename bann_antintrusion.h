#ifndef BANN_ANTINTRUSION_H
#define BANN_ANTINTRUSION_H

#include "bann1_button.h" // bannOnIcons
#include "bann3_buttons.h" // bann3ButLab
#include "device_status.h"

#include <QString>
#include <QList>

/// Forward Declarations
class Keypad;
class device;
class QDomNode;


class BannSingleLeft : public BannerNew
{
Q_OBJECT
protected:
	enum States
	{
		PARTIAL_ON,
		PARTIAL_OFF,
	};

	BannSingleLeft(QWidget *parent = 0);
	void initBanner(const QString &_left_on, const QString &_left_off, const QString &_center_on,
		const QString &_center_off, const QString &zone, States init_state, const QString &banner_text);
	void setState(States new_state);

	BtButton *left_button;

private:
	QLabel *zone_icon, *center_icon, *text;
	QString left_on, left_off, center_on, center_off;
};



// TODO: quick replacement for zonaAnti, to be cleaned up
class AntintrusionZone : public BannSingleLeft
{
Q_OBJECT
public:
	AntintrusionZone(const QDomNode &config_node, QWidget *parent = 0);
	void inizializza(bool forza = false);
public slots:
	void status_changed(QList<device_status*>);
	void toggleParzializza();
	void abilitaParz(bool ab);
	void clearChanged();
	int getIndex();

private:
	void setParzializzaOn(bool parz);
	bool already_changed;
	bool is_on;
	device *dev;
signals:
	void partChanged(AntintrusionZone *);
};

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
	void clearChanged();
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
class impAnti : public bann3ButLab
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
	void inizializza(bool forza = false);

signals:
	void impiantoInserito();
	void abilitaParz(bool);
	void clearChanged();
	void clearAlarms();

private:
	static const int MAX_ZONE = 8;
	Keypad *tasti;
	zonaAnti *le_zone[MAX_ZONE];
	bool send_part_msg;
	bool part_msg_sent;
	bool inserting;
	QString passwd;
	device *dev;

private slots:
	void Inserisci();
	void Disinserisci();
	void Insert1();
	void Insert2();
	void Insert3();
	void DeInsert();
};

#endif
