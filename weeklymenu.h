/*!
 * \weeklymenu.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief
 *
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef WEEKLYMENU_H
#define WEEKLYMENU_H

#include "sottomenu.h"
#include "device.h"

#include <qdom.h>


class WeeklyMenu : public sottoMenu
{
Q_OBJECT
public:
	WeeklyMenu(QWidget *parent, const char *name, QDomNode conf);
public slots:
	void status_changed(QPtrList<device_status> list);
private:
	void createSummerBanners();
	enum Season {SUMMER, WINTER};
	Season status;
	QDomNode conf_root;
};
#endif // WEEKLYMENU_H
