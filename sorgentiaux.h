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
class device;

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
	sorgente_aux(QWidget *parent,const char *name,char* indirizzo, bool vecchio=true, char *ambdescr="");
	void inizializza(bool forza = false);
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
public slots:
	void gestFrame(char*);
	void hide();
protected:
	device *dev;
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
private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
public:
	sorgenteMultiAux(QWidget *parent=0, const char *name=NULL, char *indirizzo="", char* Icona1="",char *Icona2="", char *Icona3="", char *ambdescr="");
	void addAmb(char *);
public slots:
	void attiva();
	void ambChanged(const QString &, bool, char *);
signals:
	void active(int, int);
};

#endif
