/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef HEADER_NAME_H
#define HEADER_NAME_H

#include "sottomenu.h"
#include "qdom.h"

class PlantMenu : public sottoMenu
{
public:
	PlantMenu(QWidget *parent, char *name, QDomNode conf);
private:
	QDomNode conf_root;
	QString ind_centrale;
};
#endif // HEADER_NAME_H
