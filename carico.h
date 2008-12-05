/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** carico.h
 **
 **
 **
 ****************************************************************/

#ifndef CARICO_H
#define CARICO_H

#include "bannondx.h" // class bannOnSx
#include "sottomenu.h"


/*!
 * \class bannCarico
 * \brief This class is made to force an appliance.
 *
 * \author Davide
 * \date lug 2005
 */
class bannCarico : public bannOnSx
{
Q_OBJECT
public:
	bannCarico(sottoMenu *parent, QString indirizzo, QString IconaSx);
	void inizializza(bool forza = false);
private slots:
	void Attiva();
public slots:
	void gestFrame(char*);
};


class Carico : public sottoMenu
{
Q_OBJECT
public:
	Carico(QWidget *parent, QDomNode config_node);
private:
	void loadItems(QDomNode config_node);
};

#endif
