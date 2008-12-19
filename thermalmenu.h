/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief  A class to handle thermal regulation menu
 *
 *  This class represents the first level menu in thermal regulation.
 *  It's instantiated in xmlconfhandler.cpp and it reads by itself the
 *  configuration.
 *  It also creates all the necessary objects and submenus to handle
 *  all the items present in a thermal regulation page.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#ifndef BTOUCH_THERMALMENU_H
#define BTOUCH_THERMALMENU_H

#include "sottomenu.h"
#include "bannpuls.h"
#include "bttime.h"

#include <QDomNode>
#include <QWidget>
#include <QString>

class FSBannTime;
class FSBannDate;

class ThermalMenu : public sottoMenu
{
Q_OBJECT
public:
	/**
	 * 
	 */
	ThermalMenu(QDomNode config_node);

public slots:
	/**
	 * Show ThermalMenu if there are two or more banners, show the only submenu below us
	 * otherwise.
	 */
	virtual void showPage();

private:
	bannPuls *addMenuItem(QDomElement, QString);
	/**
	 * Create a sottoMenu to show external and not controlled probes
	 *
	 * \param config    The node in the Dom tree that holds the `probe' section
	 * \param bann      A pointer to the banner that gives access to the
	 * sottoMenu
	 * \param external  True if the probe is external, false otherwise
	 */
	void createProbeMenu(QDomNode config, bannPuls *bann, bool external);

	void createPlantMenu(QDomNode config, bannPuls *bann);
	void loadBanners(QDomNode config_node);

	/// do NOT setAutoDelete(true), since banners are children of
	/// ThermalMenu and will be deleted by Qt

	unsigned bann_number;
	/// A reference to the only submenu below us
	sottoMenu *single_submenu;
};

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
	ProgramMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners() = 0;
	virtual void createWinterBanners() = 0;
	void setSeason(Season new_season);
protected:
	int season;
	QDomNode conf_root;
	/**
	 * \param season Either "summer" or "winter"
	 * \param what Either "prog" or "scen"
	 */
	void createSeasonBanner(QString season, QString what, QString icon);
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
	WeeklyMenu(QWidget *parent, QDomNode conf);
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
	ScenarioMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

/**
 * A submenu that let the user choose the time.
 */
class TimeEditMenu : public sottoMenu
{
Q_OBJECT
public:
	TimeEditMenu(QWidget *parent=0);
	BtTime time();
private:
	FSBannTime *time_edit;
private slots:
	void performAction();
signals:
	void timeSelected(BtTime);
};

/**
 * A submenu that let the users choose a date and emits a signal with the selected date when the user
 * confirms the choice.
 */
class DateEditMenu : public sottoMenu
{
Q_OBJECT
public:
	DateEditMenu(QWidget *parent=0);
	QDate date();
private:
	FSBannDate *date_edit;
private slots:
	void performAction();
signals:
	void dateSelected(QDate);
};

#endif
