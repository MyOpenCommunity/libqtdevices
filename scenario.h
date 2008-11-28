/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Scenario section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#ifndef SCENARIO_H
#define SCENARIO_H

#include "sottomenu.h"

#include <QWidget>

class QDomNode;


class Scenario : public sottoMenu
{
Q_OBJECT
public:
	Scenario(QWidget *parent, QDomNode config_node);
private:
	QList<scenEvo_cond*> loadConditions(QDomNode config_node);
signals:
	void resettimer();
};


#endif
