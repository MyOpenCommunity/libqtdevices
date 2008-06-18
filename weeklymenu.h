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


class ProgramMenu : public sottoMenu
{
Q_OBJECT
public:
	ProgramMenu(QWidget *parent, const char *name, QDomNode conf);
	virtual void createSummerBanners() = 0;
	virtual void createWinterBanners() = 0;
public slots:
	void status_changed(QPtrList<device_status> list);
protected:
	enum Season {SUMMER, WINTER};
	Season status;
	QDomNode conf_root;
signals:
	void programClicked(int);
};

class WeeklyMenu : public ProgramMenu
{
Q_OBJECT
public:
	WeeklyMenu(QWidget *parent, const char *name, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};
#endif // WEEKLYMENU_H
