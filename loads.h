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

#include "bann1_button.h" // class bannOnSx
#include "page.h"

class QDomNode;

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
	bannLoads(Page *parent, QString indirizzo, QString IconaSx);
private slots:
	void Attiva();
};


class Loads : public BannerPage
{
Q_OBJECT
public:
	Loads(const QDomNode &config_node);
private:
	void loadItems(const QDomNode &config_node);
};

#endif
