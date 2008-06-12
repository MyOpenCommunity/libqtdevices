/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** amplificatori.h
 **
 **definizioni dei vari amplificatori implementati
 **
 ****************************************************************/

#ifndef AMPLIFICATORI_H
#define AMPLIFICATORI_H

#include "bannregolaz.h"
#include "device.h"

class sottoMenu;

/*****************************************************************
 **Amplificatore
 ****************************************************************/
/*!
 * \class amplificatore
 * \brief This class is made to manage an audio amplifier.
 *
 * This class is quite similar to \a dimmer's one; it only has to deal with different \a Open \a messages.
 * \author Davide
 * \date lug 2005
 */
class amplificatore : public bannRegolaz
{
Q_OBJECT
private:
	char value;
	device *dev;
public:
	amplificatore( QWidget *, const char *,char*,char*,char*,char*,char*);
	void inizializza(bool forza = false);
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
public slots:
	void status_changed(QPtrList<device_status>);
};


/*****************************************************************
 **gruppo amplificatori
 ****************************************************************/
/*!
 * \class grAmplificatori
 * \brief This class is made to manage a number of audio amplifier.
 *
 * It behaves essentially like \a amplificatore even if it doesn't represent the amplifiter's state since different
 * amplifiers can have different states.
 * \author Davide
 * \date lug 2005
 */
class grAmplificatori : public bannRegolaz
{
Q_OBJECT
private:
	QPtrList<QString> elencoDisp;
public:
	grAmplificatori  ( QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);
	void inizializza(bool forza = false);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
	void setAddress(void*);
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
};

/*****************************************************************
 **Power amplifier
 ****************************************************************/
/*!
 * \class PowerAmplifier
 * \brief This class is made to manage a power audio amplifier.
 *
 */
class PowerAmplifier : public bannRegolaz
{
Q_OBJECT
public:
	PowerAmplifier(QWidget *parent, const char *name, char* indirizzo, char* onIcon, char* offIcon, char *onAmpl, char *offAmpl, char* settingIcon);
	void setBGColor(QColor c);
	void setFGColor(QColor c);

private slots:
	void showSettings();
	void toggleStatus();
	void turnUp();
	void turnDown();
private:
	QString off_icon, on_icon;
	bool status;
	sottoMenu *settings_page;
};

#endif
