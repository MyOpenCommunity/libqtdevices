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

#include <QWidget>
#include <QString>
#include <QList>

class device;
class openwebnet;

/*!
 * \class dimmer
 * \brief This is the dimmer-banner class.
 * \author Davide
 * \date lug 2005
 */
class dimmer : public bannRegolaz
{
Q_OBJECT
public:
	dimmer(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon, QString inactiveIcon, QString breakIcon, bool to_be_connect=true);
	virtual void inizializza(bool forza = false);
	void Draw();
protected:
	char pul;
	char gruppi[4];
	device *dev;
private slots:
	virtual void Accendi();
	virtual void Spegni();
	virtual void Aumenta();
	virtual void Diminuisci();
public slots:
	virtual void status_changed(QList<device_status*>);
signals:
	void frame_available(char *);
};


/*!
 * \class dimmer 100 livelli
 * \brief This is the 100 lev dimmer-banner class.
 * \author Ciminaghi
 * \date Mar 2006
 */
class dimmer100 : public dimmer
{
Q_OBJECT;
public:
	dimmer100(QWidget *parent, QString where, QString IconaSx, QString IconaDx, QString icon,
		QString inactiveIcon, QString breakIcon, int sstart, int sstop);
	void inizializza(bool forza=false);
	void status_changed(QList<device_status*>);
private:
	/*!
	 * \brief decode msg code, lev and speed from open message
	 * returns true if msg_open is a message for a new dimmer,
	 * false otherwise
	 */
	bool decCLV(openwebnet&, char& code, char& lev, char& speed, char& h, char& m, char& s);
	int softstart, softstop;
	int last_on_lev;
	int speed;
private slots:
	void Accendi();
	void Spegni();
	void Aumenta();
	void Diminuisci();
};


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
public:
	grDimmer(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx, QString Iconsx, QString Icondx);
	void inizializza(bool forza = false);
protected:
	QList<QString> elencoDisp;
private slots:
	virtual void Attiva();
	virtual void Disattiva();
	virtual void Aumenta();
	virtual void Diminuisci();
private:
	void sendFrame(QString msg);
};


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
public:
	grDimmer100(QWidget *parent, QList<QString> addresses, QString IconaSx, QString IconaDx,QString Iconsx,
		QString Icondx, QList<int> sstart, QList<int> sstop);
private:
	QList<int> soft_start;
	QList<int> soft_stop;
private slots:
	void Attiva();
	void Disattiva();
	void Aumenta();
	void Diminuisci();
};

#endif
