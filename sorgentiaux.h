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
public:
	sorgente_aux(QWidget *parent, QString aux_name, QString indirizzo, bool vecchio=true, QString ambdescr=QString());
	virtual ~sorgente_aux();
	void inizializza(bool forza = false);
protected:
	virtual void hideEvent(QHideEvent *event);
	aux *myAux;
private slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
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
	sorgenteMultiAux(QWidget *parent, QString aux_name, QString indirizzo, QString Icona1,
		QString Icona2, QString Icona3, QString ambdescr=QString());
	void addAmb(QString a);
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
