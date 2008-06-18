/*!
 * \holidaymenu.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#include "holidaymenu.h"
#include "bannfullscreen.h"

#include "main.h"

HolidayMenu::HolidayMenu(QWidget *parent, const char *name)
	: sottoMenu(parent, name, 4, MAX_WIDTH, MAX_HEIGHT, 1)  // submenu with one item per page
{
	// create custom widget that for every click
	// checks if next date is valid
	// if yes, it displays the nember
	// FIXME: modify WeeklyMenu to be used also here.
}
