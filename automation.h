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

#include "page.h"

#include <QWidget>

class QDomNode;
class banner;


class Automation : public BannerPage
{
Q_OBJECT
public:
	Automation(const QDomNode &config_node);

	virtual int sectionId();

	banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};

#endif
