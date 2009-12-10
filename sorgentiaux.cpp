/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sorgentiaux.cpp
 **
 **definizioni delle sorgenti aux implementate
 **
 ****************************************************************/

#include "sorgentiaux.h"
#include "main.h" // ICON_CICLA, ICON_FFWD, ICON_REW
#include "aux.h" // class Aux
#include "generic_functions.h" // createMsgOpen

#include <QWidget>
#include <QDebug>


sorgente_aux::sorgente_aux(QWidget *parent, QString indirizzo, bool vecchio)
	: bannCiclaz(parent, vecchio ? 4 : 3)
{
	SetIcons(ICON_CICLA, QString(), ICON_FFWD, ICON_REW);
	setAddress(indirizzo);

	if (vecchio)
	{
		connect(this, SIGNAL(sxClick()), this, SLOT(ciclaSorg()));
		connect(this, SIGNAL(csxClick()), this, SLOT(decBrano()));
		connect(this, SIGNAL(cdxClick()), this, SLOT(aumBrano()));
		nascondi(BUT2);
	}
}

void sorgente_aux::ciclaSorg()
{
	sendFrame(QString("*22*22#4#1*5#2#%1##").arg(getAddress().at(2)));
}

void sorgente_aux::decBrano()
{
	sendFrame(QString("*22*10*2#%1##").arg(getAddress().at(2)));
}

void sorgente_aux::aumBrano()
{
	sendFrame(QString("*22*9*2#%1##").arg(getAddress().at(2)));
}


sorgenteMultiAux::sorgenteMultiAux(QWidget *parent, QString aux_name, QString indirizzo, QString Icona1, QString Icona2,
	QString Icona3, QString ambdescr) : sorgente_aux(parent, indirizzo, false)
{
	qDebug("sorgenteMultiAux::sorgenteMultiAux()");
	SetIcons(Icona1, Icona2, QString(), Icona3);
	indirizzo_semplice = indirizzo;
	indirizzi_ambienti.clear();
	myAux = new Aux(aux_name, ambdescr);
	connect(this, SIGNAL(dxClick()), myAux, SLOT(showPage()));
	connect(this, SIGNAL(sxClick()), SLOT(attiva()));
	connect(myAux, SIGNAL(Closed()), SIGNAL(pageClosed()));
	connect(myAux, SIGNAL(Btnfwd()), SLOT(aumBrano()));
	multiamb = false;
}

sorgenteMultiAux::~sorgenteMultiAux()
{
	delete myAux;
}

void sorgenteMultiAux::attiva()
{
	qDebug("sorgenteMultiAux::attiva()");

	if (!multiamb)
	{
		QString f = QString("*22*35#4#%1#%2*3#%1#0##").arg(indirizzo_ambiente).arg(indirizzo_semplice.toInt());
		sendFrame(f);
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	}
	else
	{
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI", indirizzi_ambienti.count());
		for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		{
			sendFrame("*22*35#4#" + *it + "#" + indirizzo_semplice + "*3#" + *it + "#0##");
		}
	}
}

void sorgenteMultiAux::ambChanged(const QString & ad, bool multi, QString indamb)
{
	qDebug() << "sorgenteMultiRadio::ambChanged(" << ad << ", " << multi << ", " << indamb << ")";
	if (!multi)
	{
		multiamb = false;
		indirizzo_ambiente = indamb.toInt();
		QString dove(QString::number(100 + indamb.toInt() * 10 + indirizzo_semplice.toInt(), 10));
		qDebug() << "Source where is now " << dove;
		setAddress(dove);
	}
	else
	{
		QString dove(QString::number(100 + indirizzo_semplice.toInt(), 10));
		qDebug() << "Source where is now " << dove;
		setAddress(dove);
		multiamb = true;
	}
	myAux->setAmbDescr(ad);
}

void sorgenteMultiAux::addAmb(QString a)
{
	for (QStringList::Iterator it = indirizzi_ambienti.begin(); it != indirizzi_ambienti.end(); ++it)
		if (*it == a) 
			return;
	qDebug() << "sorgenteMultiAux::addAmb" << a;
	indirizzi_ambienti += a;
}
