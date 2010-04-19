/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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


/**
 * Banner for radio sources.
 */
class RadioSource : public BannerNew
{
Q_OBJECT
public:
	RadioSource();

protected:
	void initBanner(const QString &left, const QString &center_left, const QString &center, const QString &center_right,
		const QString &right);


private:
	BtButton *left_button, *center_left_button, *center_right_button, *right_button;
	// TODO: this must be changed with the proper widget
	QLabel *dummy;
};

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
 */
class banradio : public bannCiclaz
{
Q_OBJECT
public:
	banradio(QWidget *parent, QString indirizzo, int nbut=4, const QString &ambdescr=tr("Default environment"));
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
 */
class sorgenteMultiRadio : public banradio
{
Q_OBJECT
public:
	sorgenteMultiRadio(QWidget *parent, QString indirizzo, QString Icona1, QString Icona2, QString Icona3, const QString &ambDescr="");

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
