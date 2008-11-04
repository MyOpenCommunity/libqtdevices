/*!
 * \brightnesspage.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A widget to select the brightness level
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BRIGHTNESSPAGE_H
#define BRIGHTNESSPAGE_H

#include "sottomenu.h"

class BrightnessPage : public sottoMenu
{
Q_OBJECT
public:
	BrightnessPage(QWidget *parent = 0);
};

#endif // BRIGHTNESSPAGE_H
