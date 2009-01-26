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

#include "bannonoff.h"
#include "bann1_button.h"
#include "main.h" // MAX_PATH

class device;
class device_status;

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
	attuatAutom(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString IconActive,
		QString IconDisactive);
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
	attuatPuls(QWidget *parent, QString where, QString IconaSx, QString IconActive, char tipo);
private slots:
	void Attiva();
	void Disattiva();
private:
	char type;
};

#endif
