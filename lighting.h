/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Lighting section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#ifndef LIGHTING_H
#define LIGHTING_H

#include "sottomenu.h"

#include <QWidget>

class QDomNode;


class Lighting : public sottoMenu
{
Q_OBJECT
public:
	Lighting(QWidget *parent, QDomNode config_node);

protected:
	virtual void showEvent(QShowEvent *event);

private:
	void init_dimmer();
};

#endif
