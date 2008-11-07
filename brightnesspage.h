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
#include "bannondx.h"
#include "brightnesscontrol.h"

#include <QButtonGroup>

class BannToggle : public bannOnSx
{
Q_OBJECT
public:
	BannToggle(sottoMenu *parent=0, const char *name=0);
	BtButton *getButton();
	~BannToggle() { };
};

class BrightnessPage : public sottoMenu
{
Q_OBJECT
public:
	BrightnessPage(QWidget *parent = 0);
public slots:
	void brightnessSelected();
private:
	BannToggle *getBanner(const QString &banner_text);
	QButtonGroup buttons;
};

#endif // BRIGHTNESSPAGE_H
