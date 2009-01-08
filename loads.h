/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** loads.h
 **
 **
 **
 ****************************************************************/

#ifndef LOADS_H
#define LOADS_H

#include "bannondx.h" // class bannOnSx
#include "sottomenu.h"


/*!
 * \class bannLoads
 * \brief This class is made to force an appliance.
 *
 * \author Davide
 * \date lug 2005
 */
class bannLoads : public bannOnSx
{
Q_OBJECT
public:
	bannLoads(sottoMenu *parent, QString indirizzo, QString IconaSx);
private slots:
	void Attiva();
public slots:
	void gestFrame(char*);
};


class Loads : public sottoMenu
{
Q_OBJECT
public:
	Loads(QDomNode config_node);
private:
	void loadItems(QDomNode config_node);
};

#endif
