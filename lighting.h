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

#include "page.h"

#include <QWidget>

class QDomNode;
class banner;


class Lighting : public BannerPage
{
Q_OBJECT
public:
	Lighting(const QDomNode &config_node);

	virtual int sectionId();

	banner *getBanner(const QDomNode &item_node);

protected:
	virtual void showEvent(QShowEvent *event);

private:
	void initDimmer();
	void loadItems(const QDomNode &config_node);
};

#endif
