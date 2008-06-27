/*!
 * \weeklymenu.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A collection of specialized menus.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef WEEKLYMENU_H
#define WEEKLYMENU_H

#include "sottomenu.h"
#include "device_status.h"
#include "bannfullscreen.h"

#include <qdom.h>

/**
 * A base class for submenus that allow to choose one program in a list. The list changes
 * when season changes (summer/winter).
 * This class emits a signal when a program is clicked. This signal should be used to close
 * the submenu and to take further action, for example sending a frame to the thermal regulator.
 */
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

/**
 * This is a specialized version of ProgramMenu to select week programs. The list
 * of programs is read from DOM.
 */
class WeeklyMenu : public ProgramMenu
{
Q_OBJECT
public:
	WeeklyMenu(QWidget *parent, const char *name, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

/**
 * This is a specialized version of ProgramMenu to select scenarios. The list
 * of scenarios is read from DOM and updated when season changes
 */
class ScenarioMenu : public ProgramMenu
{
Q_OBJECT
public:
	ScenarioMenu(QWidget *parent, const char *name, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

class TimeEditMenu : public sottoMenu
{
Q_OBJECT
public:
	TimeEditMenu(QWidget *parent, const char *name);
	QTime time();
private:
	FSBannTime *time_edit;
signals:
	void timeChanged(QTime);
};

class DateEditMenu : public sottoMenu
{
Q_OBJECT
public:
	DateEditMenu(QWidget *parent, const char *name);
	QDate date();
private:
	FSBannDate *date_edit;
signals:
	void dateChanged(QDate);
};
#endif // WEEKLYMENU_H
