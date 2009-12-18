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
class banner;


class Scenario : public sottoMenu
{
Q_OBJECT
public:
	Scenario(const QDomNode &config_node);
	banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
	int section_id;
};

#endif
