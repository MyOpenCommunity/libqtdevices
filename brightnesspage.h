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

#include <QButtonGroup>


class BrightnessPage : public sottoMenu
{
Q_OBJECT
public:
	BrightnessPage(QWidget *parent = 0);
public slots:
	void brightnessSelected();

private:
	void addBanner(const QString &text, int id);
	QButtonGroup buttons;
};

#endif // BRIGHTNESSPAGE_H
