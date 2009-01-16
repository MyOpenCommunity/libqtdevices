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

#include "singlechoicepage.h"

/*
 * The page used to choose the Brightness value.
 */
class BrightnessPage : public SingleChoicePage
{
Q_OBJECT
public:
	BrightnessPage();

protected:
	virtual int getCurrentId();
	virtual void bannerSelected(int id);
};

#endif // BRIGHTNESSPAGE_H
