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

#include "page.h"

#include <QWidget>
#include <QList>

class QDomNode;
class scenEvo_cond;


class Scenario : public BannerPage
{
Q_OBJECT
public:
	Scenario(const QDomNode &config_node);
private:
	QList<scenEvo_cond*> loadConditions(const QDomNode &config_node);
	void loadItems(const QDomNode &config_node);
};


#endif
