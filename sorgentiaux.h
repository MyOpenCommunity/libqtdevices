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


#ifndef SORGENTI_AUX_H
#define SORGENTI_AUX_H

#include "bannciclaz.h"

#include <QString>
#include <QStringList>

/// Forward Declarations
class Aux;

/*****************************************************************
 **Sorgente_AUX
 ****************************************************************/
/*!
 * \class sorgente_aux
 * \brief This class is made to manage an auxiliary sound source.
 *
 * This particoular banner is composed by 2 buttons from which is possible: change the sound source,  go to the next track.
 * \date lug 2005
 */
class sorgente_aux : public bannCiclaz
{
Q_OBJECT
public:
	sorgente_aux(QWidget *parent, QString indirizzo, bool vecchio=true);

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
 * \date jul 2006
 */
class sorgenteMultiAux : public sorgente_aux
{
Q_OBJECT
public:
	sorgenteMultiAux(QWidget *parent, QString aux_name, QString indirizzo, QString Icona1,
		QString Icona2, QString Icona3, QString ambdescr=QString());
	virtual ~sorgenteMultiAux();
	void addAmb(QString a);

public slots:
	void attiva();
	void ambChanged(const QString &, bool, QString);

signals:
	void active(int, int);

private:
	QString indirizzo_semplice;
	QStringList indirizzi_ambienti;
	bool multiamb;
	int indirizzo_ambiente;
	Aux *myAux;
};

#endif
