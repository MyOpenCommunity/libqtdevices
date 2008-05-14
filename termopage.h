/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** termopage.h
 **
 **definizioni della zona di termoregolazione
 **
 ****************************************************************/

#ifndef TERMOPAGE_H
#define TERMOPAGE_H

#include "banntermo.h"
#include "device.h"

#include <qtimer.h>

/*****************************************************************
 **Zona Termoregolazione
 ****************************************************************/
/*!
 * \class termoPage
 * \brief This class is made to manage a thermoregulation zone.
 *
 * By this banner is possible to see the measured temperature, the set point, the offset locally setted and, if permitted
 * by the central, to switch from automatic and manual control. When manual controlling the zone it is possible to change
 * the setpoint.
 * \author Davide
 * \date lug 2005
 */
class termoPage : public bannTermo
{
Q_OBJECT
private:
	/// Tipo Centrale: 0=3550 (99 zone), 1=4695 (4 zone)
	int type;

	QString ind_centrale;
	QString indirizzo;

	device *dev;
	char manIco[50];
	char autoIco[50];
	QTimer setpoint_timer;
public:
	termoPage (QWidget *parent, devtype_t devtype, const char *name, const char *indirizzo,
		QPtrList<QString> &icon_names,
		QColor SecondForeground = QColor(0,0,0), int type = 0, const char *ind_centrale = 0);
	void inizializza(bool forza = false);
	char* getChi();
	char* getAutoIcon();
	char* getManIcon();
	void SetSetpoint(float icx);
	int delta_setpoint;
private slots:
	void aumSetpoint();
	void decSetpoint();
	void autoMan();
	void sendSetpoint();
	void handleFancoilButtons(int button_number);
public slots:
	void status_changed(QPtrList<device_status>);
	void show();
	void hide();
};

#endif
