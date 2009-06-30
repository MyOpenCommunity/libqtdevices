/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentiradio.h
 **
 **definizioni delle sorgenti radio implementate
 **
 ****************************************************************/

#ifndef SORGENTI_RADIO_H
#define SORGENTI_RADIO_H

#include "bannciclaz.h"
#include "device_status.h"

#include <QList>
#include <QString>
#include <QStringList>

/// Forward Declarations
class radio;
class device;

/*****************************************************************
 **Sorgente_Radio
 ****************************************************************/
/*!
 * \class banradio
 * \brief This class is made to manage the FM tuner.
 *
 * This particoular banner is composed by 4 buttons from which is possible: change the sound source, open an toher page
 * where to watch the detailed information about the tuner (frequency, RDS message, ...), go to the next/previous
 * memorized frequency.
 * \author Davide
 * \date lug 2005
 */
class banradio : public bannCiclaz
{
Q_OBJECT
public:
	banradio(QWidget *parent, QString indirizzo, int nbut=4, const QString & ambdescr="");
	void inizializza(bool forza = false);

protected:
	radio* myRadio;
	bool old_diffson;
	device *dev;
	void pre_show();
	virtual void showEvent(QShowEvent *event);

protected slots:
	void ciclaSorg();
	void decBrano();
	void aumBrano();
	void aumFreqAuto();
	void decFreqAuto();
	void aumFreqMan();
	void decFreqMan();
	void changeStaz();
	void memoStaz(uchar);
	void richFreq();
	void stopRDS();
	void startRDS();

public slots:
	void status_changed(QList<device_status*>);
	virtual void setText(const QString &);
};


/*****************************************************************
 ** Sorgente radio diffusione sonora multicanale
 ****************************************************************/
/*!
 * \class sorgenteMultiRadio
 * \brief This class represents a radio source in the multi-channel audio diff. sys.
 * \author Ciminaghi
 * \date jul 2006
 */
class sorgenteMultiRadio : public banradio
{
Q_OBJECT
public:
	sorgenteMultiRadio(QWidget *parent, QString indirizzo, QString Icona1, QString Icona2, QString Icona3, char *ambDescr="");

public slots:
	void attiva();
	void addAmb(QString a);
	void ambChanged(const QString &, bool, QString);

protected:
	virtual void showEvent(QShowEvent *event);

private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;

signals:
	void active(int, int);
};

#endif
