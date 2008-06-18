/*!
 * \holidaymenu.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Submenu for thermal regulator settings to handle holiday settings
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef HOLIDAYMENU_H
#define HOLIDAYMENU_H

#include "sottomenu.h"

#include <qlayout.h>

class HolidayMenu : public sottoMenu
{
Q_OBJECT
public:
	HolidayMenu(QWidget *parent, const char *name);
private:
};

#endif // HOLIDAYMENU_H
