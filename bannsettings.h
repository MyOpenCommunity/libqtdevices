/*!
 * \bannsettings.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNSETTINGS_H
#define BANNSETTINGS_H

#include "bann3but.h"
#include "bann4but.h"

class BannOff : public bann3But
{
Q_OBJECT
public:
	BannOff(QWidget *parent, const char *name);
public slots:
	/**
	 * Shut down the thermal regulator
	 */
	void performAction();
private:
};

class BannSummerWinter : public bann4But
{
Q_OBJECT
public:
	BannSummerWinter(QWidget *parent, const char *name);
	enum seasons {WINTER, SUMMER};
public slots:
	void setSummer();
	void setWinter();
private:
	seasons status;
};

#endif // BANNSETTINGS_H
