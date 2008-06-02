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
#include "device.h"

/// Forward Declarations
class radio;

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
protected:
	void pre_show();
	radio* myRadio;
	bool old_diffson;
	device *dev;
public:
	banradio( QWidget *parent,const char *name,char* indirizzo, int nbut=4, const QString & ambdescr="");
	void inizializza(bool forza = false);
	/*!
	  \brief Sets the background color for the banner.

	  The arguments are RGB components for the color.
	*/
	void setBGColor(int, int , int );
	/*!
	  \brief Sets the foreground color for the banner.

	  The arguments are RGB components for the color.
	*/
	void setFGColor(int , int , int );
	/*!
	  \brief Sets the background color for the banner.

	  The argument is the QColor description of the color.
	*/
	void setBGColor(QColor );
	/*!
	  \brief Sets the foreground color for the banner.

	  The argument is the QColor description of the color.
	*/
	void setFGColor(QColor );
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
	void grandadChanged(QWidget *);
public slots:
	void status_changed(QPtrList<device_status>);
	virtual void	show();
	void hide();
	void SetTextU( const QString & );
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
private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
public:
	sorgenteMultiRadio(QWidget *parent=0, const char *name=NULL, char *indirizzo="", char* Icona1="",char *Icona2="", char *Icona3="", char *ambDescr="");
public slots:
	void attiva();
	void addAmb(char *);
	void ambChanged( const QString &, bool, char *);
	void show();
signals:
	void active(int, int);
};

#endif
