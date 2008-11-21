/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentiaux.h
 **
 **definizioni delle sorgenti aux implementate
 **
 ****************************************************************/

#ifndef SORGENTI_AUX_H
#define SORGENTI_AUX_H

#include "bannciclaz.h"

#include <QString>
#include <QStringList>

/// Forward Declarations
class aux;

/*****************************************************************
 **Sorgente_AUX
 ****************************************************************/
/*!
 * \class sorgente_aux
 * \brief This class is made to manage an auxiliary sound source.
 *
 * This particoular banner is composed by 2 buttons from which is possible: change the sound source,  go to the next track.
 * \author Davide
 * \date lug 2005
 */
class sorgente_aux : public bannCiclaz
{
Q_OBJECT
protected:
	aux *myAux;
	bool vecchia;
public:
	sorgente_aux(QWidget *parent, QString aux_name, char* indirizzo, bool vecchio=true, QString ambdescr=QString());
	void inizializza(bool forza = false);
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
public slots:
	void gestFrame(char*);
protected:
	virtual void hideEvent(QHideEvent *event);
};


/*****************************************************************
 ** Sorgente aux diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class sorgenteMultiAux
 * \brief This class represents an aux source in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class sorgenteMultiAux : public sorgente_aux
{
Q_OBJECT
public:
	sorgenteMultiAux(QWidget *parent=0, QString aux_name=QString(), char *indirizzo="", QString Icona1=QString(),
		QString Icona2=QString(), QString Icona3=QString(), QString ambdescr=QString());
	void addAmb(char *);
public slots:
	void attiva();
	void ambChanged(const QString &, bool, QString);
private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
signals:
	void active(int, int);
};

#endif
