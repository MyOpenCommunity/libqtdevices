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

#include <QWidget>
#include <QString>
#include <QList>

class device_status;
class device;


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
public:
	amplificatore(QWidget *parent, QString indirizzo, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon);
	void inizializza(bool forza = false);
public slots:
	void status_changed(QList<device_status*>);
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
private:
	char value;
	device *dev;
};


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
	grAmplificatori(QWidget *parent=0, QList<QString> indirizzi=QList<QString>(), QString IconaSx=QString(), QString IconaDx=QString(), QString Iconsx=QString(), QString Icondx=QString());
	void inizializza(bool forza = false);
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
private:
	QList<QString> elencoDisp;
	void sendActivationFrame(QString argm);
};

#endif
