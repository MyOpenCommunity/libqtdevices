/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** dimmers.h
 **
 **definizioni dei vari dimmers implementati
 **
 ****************************************************************/

#ifndef DIMMERS_H
#define DIMMERS_H

#include "device_status.h"
#include "bannregolaz.h"


class device;

/*****************************************************************
 **Dimmer
 ****************************************************************/
/*!
 * \class dimmer
 * \brief This is the dimmer-banner class.
 * \author Davide
 * \date lug 2005
 */
class dimmer : public bannRegolaz
{
Q_OBJECT
protected:
	char pul;
	char gruppi[4];
	device *dev;
public:
	dimmer(QWidget *, const char *, char *, char *, char *, char *, char *, char *, bool to_be_connect = true);
	virtual void inizializza(bool forza = false);
	void Draw();
private slots:
	virtual void Accendi();
	virtual void Spegni();
	virtual void Aumenta();
	virtual void Diminuisci();
public slots:
	virtual void status_changed(QPtrList<device_status>);
signals:
	void frame_available(char *);
};


/*****************************************************************
 **Dimmer 100 Livelli
 ****************************************************************/
/*!
 * \class dimmer 100 livelli
 * \brief This is the 100 lev dimmer-banner class.
 * \author Ciminaghi
 * \date Mar 2006
 */
class dimmer100 : public dimmer
{
Q_OBJECT;
private:
	/*!
	 * \brief decode msg code, lev and speed from open message
	 * returns true if msg_open is a message for a new dimmer, 
	 * false otherwise
	 */
	bool decCLV(openwebnet&, char& code, char& lev, char& speed,
	char& h, char& m, char& s); 
	int softstart, softstop;
	int last_on_lev;
	int speed;
public:
	dimmer100(QWidget *, const char *, char *, char *, char *, char *, char *, char *, int, int);
	void inizializza(bool forza=false);
	void status_changed(QPtrList<device_status>);
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
};


/*****************************************************************
 **gruppo dimmer
 ****************************************************************/
/*!
 * \class grDimmer
 * \brief This class is made to control a number of dimmers.
 *
 * It behaves essentially like \a dimmer even if it doesn't represent the dimmer's state 
 * since different dimmers can have different states.
 * \author Davide
 * \date lug 2005
 */
class grDimmer : public bannRegolaz
{
Q_OBJECT
protected:
	QPtrList<QString> elencoDisp;
public:
	grDimmer  (QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0);
	/*! \brief This method is used to add an address list of the objects contained int he group managed by this class*/
	void setAddress(void*);
	void inizializza(bool forza = false);
private slots:
	virtual void Attiva();
	virtual void Disattiva();
	virtual void Aumenta();
	virtual void Diminuisci();
private:
	device *dev;
};


/*****************************************************************
 **gruppo dimmer 100 livelli
 ****************************************************************/
/*!
 * \class grDimmer100
 * \brief This class is made to control a number of 100 levels dimmers
 *
 * It behaves essentially like \a dimmer group
 * \author Davide
 * \date Jun 2006
 */
class grDimmer100 : public grDimmer
{
Q_OBJECT
private:
	QValueList<int> soft_start;
	QValueList<int> soft_stop;
public:
	grDimmer100  (QWidget *parent=0, const char *name=NULL ,void*indirizzi=NULL,char* IconaSx=NULL,char* IconaDx=NULL,char*Iconsx=NULL,char*Icondx=NULL,int periodo=0,int numFrame=0, QValueList<int> sstart = QValueList<int>(), QValueList<int> sstop = QValueList<int>());
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
private:
	device *dev;
};

#endif
