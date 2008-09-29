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

#include <qwidget.h>

class device_status;
class device;
class QString;

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
	amplificatore(QWidget *, const char *,char*,char*,char*,char*,char*);
	void inizializza(bool forza = false);
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
public slots:
	void status_changed(QList<device_status*>);
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
public:
	grAmplificatori(QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);
	~grAmplificatori();
	void inizializza(bool forza = false);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class */
	void setAddress(void*);
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
private:
	// TODO: rendere elencoDisp un QList<QString>!! Adesso viene costruito in ambdiffson e distrutto qui!
	QList<QString> *elencoDisp;
	device *dev;
};

#endif
