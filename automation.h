/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Automation section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#ifndef AUTOMATION_H
#define AUTOMATION_H

#include "sottomenu.h"

#include <QWidget>

class QDomNode;


class Automation : public sottoMenu
{
Q_OBJECT
public:
	Automation(QDomNode config_node);
private:
	void loadItems(QDomNode config_node);
};

#endif
