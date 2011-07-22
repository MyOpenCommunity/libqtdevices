/*
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "thermalregulator.h"
#include "thermal_device.h"
#include "fontmanager.h"
#include "btbutton.h"
#include "xml_functions.h"
#include "icondispatcher.h"
#include "scaleconversion.h"
#include "skinmanager.h"
#include "datetime.h"
#include "bann_thermal_regulation.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QtDebug>


enum
{
	BANNER_PROGRAMS = 8101,
	BANNER_SCENARIOS = 8102,
	BANNER_MANUAL_TIMED_MODE = 8321,
	BANNER_MANUAL_MODE = 8103,
	BANNER_WEEKEND_MODE = 8104,
	BANNER_HOLIDAY_MODE = 8105,
	WINTER_PROGRAM = 8201,
	SUMMER_PROGRAM = 8202,
	WINTER_SCENARIO = 8251,
	SUMMER_SCENARIO = 8252,
};

#define SUMMER_PREFIX "summer"
#define WINTER_PREFIX "winter"

namespace
{
	QLabel *getLabelWithPixmap(const QString &img, QWidget *parent, int alignment)
	{
		QLabel *tmp = new QLabel(parent);
		tmp->setPixmap(*bt_global::icons_cache.getIcon(img));
		tmp->setAlignment((Qt::Alignment)alignment);
		return tmp;
	}

	// match the order of enum Status in ThermalDevice
	static QString status_icons_ids[ThermalDevice::ST_COUNT] =
	{
		"regulator_off", "regulator_antifreeze", "regulator_manual", "regulator_manual_timed",
		"regulator_weekend", "regulator_program", "regulator_scenario", "regulator_holiday"
	};

	void parseProgramList(QDomNode page, ProgramEntries &entries)
	{
		foreach (const QDomNode &node, getChildren(page, "item"))
		{
			int id = getTextChild(node, "id").toInt();
			QString text = getTextChild(node, "descr");

			QString prefix;
			if (id == WINTER_PROGRAM || id == WINTER_SCENARIO)
				prefix = WINTER_PREFIX;
			else if (id == SUMMER_PROGRAM || id == SUMMER_SCENARIO)
				prefix = SUMMER_PREFIX;

			if (prefix.isNull())
				qWarning() << "Unknown id" << id << "for thermal regulator programs/scenarios";
			else
				entries.append(qMakePair(prefix + getTextChild(node, "num"), text));
		}
	}
}


SettingsPage::SettingsPage(QDomNode n, QWidget *parent)
	: BannerPage(parent)
{
	buildPage(getTextChild(n, "descr"));
}

void SettingsPage::appendBanner(Banner *b)
{
	page_content->appendBanner(b);
}

void SettingsPage::resetIndex()
{
	page_content->resetIndex();
}


PageManual::PageManual(ThermalDevice *d, TemperatureScale scale, QString descr)
{
	temp_scale = scale;
	dev = d;
	setpoint_delta = 5;

	QWidget *content = new QWidget;
	main_layout = new QVBoxLayout(content);
#ifdef LAYOUT_TS_10
	main_layout->setSpacing(8);
	main_layout->setContentsMargins(30, 0, 30, 18);
#else
	main_layout->setSpacing(20);
	main_layout->setContentsMargins(0, 0, 0, 10);

	QLabel *descr_label = new QLabel(descr);
	descr_label->setFont(bt_global::font->get(FontManager::TEXT));
	descr_label->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

	main_layout->addWidget(descr_label);
#endif

	switch (temp_scale)
	{
	case CELSIUS:
		maximum_manual_temp = bt2Celsius(dev->maximumTemp());
		minimum_manual_temp = bt2Celsius(dev->minimumTemp());
		temp = 200;
		break;
	case FAHRENHEIT:
		maximum_manual_temp = bt2Fahrenheit(dev->maximumTemp());
		minimum_manual_temp = bt2Fahrenheit(dev->minimumTemp());
		temp = 680;
		break;
	default:
		qWarning("BannManual ctor: wrong scale, defaulting to celsius");
		maximum_manual_temp = bt2Celsius(dev->maximumTemp());
		minimum_manual_temp = bt2Celsius(dev->minimumTemp());
		temp = 200;
		temp_scale = CELSIUS;
	}

	temp_label = new QLabel(this);
	temp_label->setFont(bt_global::font->get(FontManager::REGULATOR_TEMPERATURE));
	temp_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	temp_label->setProperty("SecondFgColor", true);
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setSpacing(20);

	BtButton *minus_btn = new BtButton(bt_global::skin->getImage("minus"));
	minus_btn->setAutoRepeat(true);
	connect(minus_btn, SIGNAL(clicked()), this, SLOT(decSetpoint()));

	BtButton *plus_btn = new BtButton(bt_global::skin->getImage("plus"));
	plus_btn->setAutoRepeat(true);
	connect(plus_btn, SIGNAL(clicked()), this, SLOT(incSetpoint()));

	hbox->addStretch(1);
	hbox->addWidget(minus_btn);
	hbox->addWidget(temp_label);
	hbox->addWidget(plus_btn);
	hbox->addStretch(1);

	main_layout->addLayout(hbox);
	main_layout->addStretch();

#ifdef LAYOUT_TS_3_5
	NavigationBar *nav_bar = new NavigationBar("ok");
	nav_bar->displayScrollButtons(false);
	buildPage(content, nav_bar);
#else
	QHBoxLayout *ok_layout = new QHBoxLayout;
	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));
	ok_layout->addWidget(ok, 0, Qt::AlignRight | Qt::AlignBottom);

	main_layout->addLayout(ok_layout);

	connect(ok, SIGNAL(clicked()), SLOT(performAction()));

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);

	buildPage(content, nav_bar, new PageTitleWidget(descr, TITLE_HEIGHT));
#endif

	connect(nav_bar, SIGNAL(forwardClick()), SLOT(performAction()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	updateTemperature();
}

void PageManual::performAction()
{
	unsigned bt_temp;
	switch (temp_scale)
	{
	case CELSIUS:
		bt_temp = celsius2Bt(temp);
		break;
	case FAHRENHEIT:
		bt_temp = fahrenheit2Bt(temp);
		break;
	default:
		qWarning("BannManual::performAction: unknown scale, defaulting to celsius");
		bt_temp = celsius2Bt(temp);
	}
	emit(temperatureSelected(bt_temp));
}

void PageManual::incSetpoint()
{
	// FIXME: forse c'e' da modificare i controlli in caso di fahrenheit
	if (temp >= maximum_manual_temp)
		return;
	else
		temp += setpoint_delta;
	updateTemperature();
}

void PageManual::decSetpoint()
{
	if (temp <= minimum_manual_temp)
		return;
	else
		temp -= setpoint_delta;
	updateTemperature();
}

void PageManual::updateTemperature()
{
	switch (temp_scale)
	{
	case CELSIUS:
		temp_label->setText(celsiusString(temp));
		break;
	case FAHRENHEIT:
		temp_label->setText(fahrenheitString(temp));
		break;
	default:
		qWarning("BannSimpleProbe: unknown temperature scale, defaulting to Celsius");
		temp_label->setText(celsiusString(temp));
	}
}

void PageManual::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(ThermalDevice::DIM_TEMPERATURE))
		return;

	unsigned temperature = values_list[ThermalDevice::DIM_TEMPERATURE].toInt();

	switch (temp_scale)
	{
	case CELSIUS:
		temp = bt2Celsius(temperature);
		break;
	case FAHRENHEIT:
		temp = bt2Fahrenheit(temperature);
		break;
	default:
		qWarning("BannSimpleProbe: unknown temperature scale, defaulting to celsius");
		temp = bt2Celsius(temperature);
	}

	updateTemperature();
}


PageManualTimed::PageManualTimed(ThermalDevice4Zones *dev, TemperatureScale scale, QString descr) : PageManual(dev, scale, descr)
{
	time_edit = new BtTimeEdit;
#ifdef LAYOUT_TS_10
	main_layout->insertWidget(2, time_edit, 0, Qt::AlignHCenter);
#else
	main_layout->insertWidget(2, time_edit);
#endif

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void PageManualTimed::performAction()
{
	unsigned bt_temp;
	switch (temp_scale)
	{
	case CELSIUS:
		bt_temp = celsius2Bt(temp);
		break;
	case FAHRENHEIT:
		bt_temp = fahrenheit2Bt(temp);
		break;
	default:
		qWarning("BannManual::performAction: unknown scale, defaulting to celsius");
		bt_temp = celsius2Bt(temp);
	}
	emit timeAndTempSelected(time_edit->time(), bt_temp);
}

void PageManualTimed::setMaxHours(int max)
{
	time_edit->setMaxHours(max);
}

void PageManualTimed::setMaxMinutes(int max)
{
	time_edit->setMaxMinutes(max);
}

PageSetDate::PageSetDate()
{
	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);

	QLabel *top = new QLabel;
	top->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("date_icon")));
	main_layout->addWidget(top, 0, Qt::AlignHCenter);

	date_edit = new BtDateEdit;
	main_layout->addWidget(date_edit);
#ifdef LAYOUT_TS_3_5
	main_layout->setSpacing(10);
#else
	main_layout->setSpacing(0);
#endif
	main_layout->setContentsMargins(0, 0, 0, 10);

	NavigationBar *nav = new NavigationBar("ok");
	nav->displayScrollButtons(false);
	buildPage(content, nav);

	connect(nav, SIGNAL(forwardClick()), SLOT(performAction()));
	connect(nav, SIGNAL(backClick()), SIGNAL(Closed()));
}

QDate PageSetDate::date()
{
	return date_edit->date();
}

void PageSetDate::performAction()
{
	emit dateSelected(date());
}


PageSetTime::PageSetTime()
{
	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);

	QLabel *top = new QLabel;
	top->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("time_icon")));
	main_layout->addWidget(top, 0, Qt::AlignHCenter);

	time_edit = new BtTimeEdit;
	main_layout->addWidget(time_edit);
#ifdef LAYOUT_TS_3_5
	main_layout->setSpacing(10);
#else
	main_layout->setSpacing(0);
#endif
	main_layout->setContentsMargins(0, 0, 0, 10);

	NavigationBar *nav = new NavigationBar("ok");
	nav->displayScrollButtons(false);
	buildPage(content, nav);

	connect(nav, SIGNAL(forwardClick()), SLOT(performAction()));
	connect(nav, SIGNAL(backClick()), SIGNAL(Closed()));
}

BtTime PageSetTime::time()
{
	return time_edit->time();
}

void PageSetTime::performAction()
{
	emit timeSelected(time());
}


PageTermoReg::PageTermoReg(QDomNode n)
{
	SkinContext context(getTextChild(n, "cid").toInt());

	icon_summer = bt_global::skin->getImage("summer_flat");
	icon_winter = bt_global::skin->getImage("winter_flat");

	for (int i = 0; i < ThermalDevice::ST_COUNT; ++i)
		status_icons.append(bt_global::skin->getImage(status_icons_ids[i]));

	// parse program/scenario list
	foreach (const QDomNode &item, getChildren(getPageNodeFromChildNode(n, "h_lnk_pageID"), "item"))
	{
		int id = getTextChild(item, "id").toInt();

		if (id == BANNER_PROGRAMS) // programs
			parseProgramList(getPageNodeFromChildNode(item, "lnk_pageID"), programs);
		else if (id == BANNER_SCENARIOS) // scenarios
			parseProgramList(getPageNodeFromChildNode(item, "lnk_pageID"), scenarios);
	}

	description_label = new QLabel(this);
	description_label->setFont(bt_global::font->get(FontManager::REGULATOR_DESCRIPTION));
	description_label->setAlignment(Qt::AlignHCenter);
	description_label->setProperty("SecondFgColor", true);

	season_icon = getLabelWithPixmap(bt_global::skin->getImage("summer_flat"), this, Qt::AlignHCenter);

	mode_icon = getLabelWithPixmap(bt_global::skin->getImage("regulator"), this, Qt::AlignHCenter);

	main_layout->addWidget(mode_icon);
	main_layout->addWidget(description_label);
	main_layout->addWidget(season_icon, 0, Qt::AlignCenter);

#ifdef LAYOUT_TS_10
	main_layout->setContentsMargins(40, 0, 40, 17);

	BtButton *settings = new BtButton(bt_global::skin->getImage("settings"));
	connect(settings, SIGNAL(clicked()), SLOT(showSettingsMenu()));

	QHBoxLayout *hbox = new QHBoxLayout;

	hbox->addStretch();
	hbox->addWidget(settings);
	main_layout->addLayout(hbox);
#endif

	main_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Maximum));
	main_layout->setAlignment(Qt::AlignHCenter);

	date_edit = 0;
	time_edit = 0;
	date_time_edit = 0;
	program_choice = 0;
	program_menu = 0;
	temp_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());

	createNavigationBar(bt_global::skin->getImage("settings"), getTextChild(n, "descr"), SMALL_TITLE_HEIGHT);
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(showSettingsMenu()));

	hideDescription();
}

void PageTermoReg::showDescription(const QString &desc)
{
	// we do not hide/show the label to avoid other page elements moving around
	description_label->setText(desc);
}

void PageTermoReg::hideDescription()
{
	// we do not hide/show the label to avoid other page elements moving around
	description_label->setText(" ");
}

void PageTermoReg::createSettingsItem(QDomNode item, SettingsPage *settings, ThermalDevice *_dev)
{
	int id = getTextChild(item, "id").toInt();

	switch (id)
	{
	case BANNER_PROGRAMS:
		weekSettings(item, settings, programs, _dev);
		break;
	case BANNER_MANUAL_MODE:
		manualSettings(item, settings, _dev);
		break;
	case BANNER_HOLIDAY_MODE:
		holidaySettings(item, settings, programs, _dev);
		break;
	case BANNER_WEEKEND_MODE:
		weekendSettings(item, settings, programs, _dev);
		break;
	default:
		qFatal("Unhandled item %d in thermal regulator settings", id);
		break;
	}
}

void PageTermoReg::valueReceived(const DeviceValues &values_list)
{
	ThermalDevice::Season season = ThermalDevice::SE_SUMMER;

	if (values_list.contains(ThermalDevice::DIM_SEASON))
		season = static_cast<ThermalDevice::Season>(values_list[ThermalDevice::DIM_SEASON].toInt());

	setSeason(season);

	int status = values_list[ThermalDevice::DIM_STATUS].toInt();
	if (status < status_icons.count())
		mode_icon->setPixmap(*bt_global::icons_cache.getIcon(status_icons[status]));

	switch (status)
	{
	// we should display the program name for holiday, but the frame is broken and reports
	// the wrong program number
	case ThermalDevice::ST_HOLIDAY:
	case ThermalDevice::ST_OFF:
	case ThermalDevice::ST_PROTECTION:
		hideDescription();
		break;
	case ThermalDevice::ST_MANUAL:
	case ThermalDevice::ST_MANUAL_TIMED:
	{
		unsigned temperature = values_list[ThermalDevice::DIM_TEMPERATURE].toInt();
		// remember: stat_var::get_val() returns an int
		QString description;
		switch (temp_scale)
		{
		case CELSIUS:
			description = celsiusString(bt2Celsius(temperature));
			break;
		case FAHRENHEIT:
			description = fahrenheitString(bt2Fahrenheit(temperature));
			break;
		default:
			qWarning("TermoReg valueReceived: unknown scale, defaulting to celsius");
			description = celsiusString(temperature);
		}
		showDescription(description);
		break;
	}
	case ThermalDevice::ST_SCENARIO:
	{
		int scenario = values_list[ThermalDevice::DIM_SCENARIO].toInt();
		QString description;
		switch (season)
		{
		case ThermalDevice::SE_SUMMER:
			description = lookupProgramDescription(SUMMER_PREFIX, "scen", scenario);
			break;
		case ThermalDevice::SE_WINTER:
			description = lookupProgramDescription(WINTER_PREFIX, "scen", scenario);
			break;
		}
		showDescription(description);
		break;
	}
	case ThermalDevice::ST_PROGRAM:
	case ThermalDevice::ST_WEEKEND:
	{
		int scenario = values_list[ThermalDevice::DIM_PROGRAM].toInt();
		QString description;
		switch (season)
		{
		case ThermalDevice::SE_SUMMER:
			description = lookupProgramDescription(SUMMER_PREFIX, "prog", scenario);
			break;
		case ThermalDevice::SE_WINTER:
			description = lookupProgramDescription(WINTER_PREFIX, "prog", scenario);
			break;
		}
		showDescription(description);
		break;
	}
	default:
		break;
	}
}

void PageTermoReg::setSeason(ThermalDevice::Season new_season)
{
	if (new_season == ThermalDevice::SE_SUMMER || new_season == ThermalDevice::SE_WINTER)
	{
		QString img;
		if (new_season == ThermalDevice::SE_SUMMER)
			img = icon_summer;
		else
			img = icon_winter;
		QPixmap *icon = bt_global::icons_cache.getIcon(img);
		season_icon->setPixmap(*icon);
		if (program_choice)
			program_choice->setSeason(new_season);
		if (program_menu)
			program_menu->setSeason(new_season);
	}
	else
		qWarning("Received season is not SUMMER or WINTER, ignoring");
}

QString PageTermoReg::lookupProgramDescription(QString season, QString what, int program_number)
{
	// summer/prog/p[program_number]
	Q_ASSERT_X(what == "prog" || what == "scen", "PageTermoReg::lookupProgramDescription",
		"'what' must be either 'prog' or 'scen'");

	ProgramEntries &entries = what == "prog" ? programs : scenarios;
	QString key = season + QString::number(program_number);

	for (int i = 0; i < entries.size(); ++i)
	{
		if (entries[i].first == key)
			return entries[i].second;
	}
	qWarning() << "Missing description for program" << key;
	return QString();
}

void PageTermoReg::createButtonsBanners(SettingsPage *settings, ThermalDevice *dev)
{
	// TODO: when we have the small button icons for TS 3.5'',
	//       remove the code to create the two separate banners
#ifdef LAYOUT_TS_10
	// off_antifreeze banner
	BannOffAntifreeze *off = new BannOffAntifreeze(settings, dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), SLOT(showPage()));
#else
	// off banner
	BannOff *off = new BannOff(settings, dev);
	settings->appendBanner(off);
	connect(off, SIGNAL(clicked()), SLOT(showPage()));

	// antifreeze banner
	BannAntifreeze *antifreeze = new BannAntifreeze(settings, dev);
	settings->appendBanner(antifreeze);
	connect(antifreeze, SIGNAL(clicked()), SLOT(showPage()));
#endif

	// summer_winter banner
	BannSummerWinter *summer_winter = new BannSummerWinter(settings, dev);
	settings->appendBanner(summer_winter);
	connect(summer_winter, SIGNAL(clicked()), SLOT(showPage()));
}

void PageTermoReg::showSettingsMenu()
{
	settings->resetIndex();
	settings->showPage();
}

//
// ------------- Utility functions to create thermal regulator settings menus -------------------
//
void PageTermoReg::manualSettings(QDomNode n, SettingsPage *settings, ThermalDevice *dev)
{
	PageManual *manual_page = new PageManual(dev, temp_scale, getTextChild(n, "descr"));

	// manual banner
	BannSinglePuls *manual = new BannSinglePuls(settings);
	manual->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_manual"),
			   getTextChild(n, "descr"));
	manual->connectRightButton(manual_page);
	settings->appendBanner(manual);

	// when operation is activated, return to probe menu
	connect(manual_page, SIGNAL(temperatureSelected(unsigned)), SLOT(manualSelected(unsigned)));
	// when operation is cancelled, return to settings page
	connect(manual, SIGNAL(pageClosed()), settings, SLOT(showPage()));
}

void PageTermoReg::manualSelected(unsigned temp)
{
	dev()->setManualTemp(temp);
	showPage();
}

void PageTermoReg::weekSettings(QDomNode n, SettingsPage *settings, ProgramEntries programs, ThermalDevice *dev)
{
	program_menu = new WeeklyMenu(programs, getTextChild(n, "descr"));

	BannSinglePuls *weekly = new BannSinglePuls(settings);
	weekly->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_program"),
			   getTextChild(n, "descr"));
	weekly->connectRightButton(program_menu);
	settings->appendBanner(weekly);

	connect(weekly, SIGNAL(pageClosed()), settings, SLOT(showPage()));
	connect(program_menu, SIGNAL(programClicked(int)), SLOT(weekProgramSelected(int)));
}

void PageTermoReg::weekProgramSelected(int program)
{
	dev()->setWeekProgram(program);
	showPage();
}

void PageTermoReg::holidaySettings(QDomNode n, SettingsPage *settings, ProgramEntries programs, ThermalDevice *dev)
{
	holiday_title = getTextChild(n, "descr");
	BannSinglePuls *bann = createHolidayWeekendBanner(settings, bt_global::skin->getImage("regulator_holiday"),
							  getTextChild(n, "descr"));
	connect(bann, SIGNAL(rightClick()), SLOT(holidaySettingsStart()));
#ifdef LAYOUT_TS_10
	if (!date_time_edit)
		date_time_edit = createDateTimeEdit(settings);
#else
	if (!date_edit)
		date_edit = createDateEdit(settings);
	if (!time_edit)
		time_edit = createTimeEdit(settings);
#endif
	if (!program_choice)
		program_choice = createProgramChoice(settings, programs, dev);
}

void PageTermoReg::weekendSettings(QDomNode n, SettingsPage *settings, ProgramEntries programs, ThermalDevice *dev)
{
	weekend_title = getTextChild(n, "descr");
	BannSinglePuls *bann = createHolidayWeekendBanner(settings, bt_global::skin->getImage("regulator_weekend"),
							  getTextChild(n, "descr"));
	connect(bann, SIGNAL(rightClick()), SLOT(weekendSettingsStart()));
#ifdef LAYOUT_TS_10
	if (!date_time_edit)
		date_time_edit = createDateTimeEdit(settings);
#else
	if (!date_edit)
		date_edit = createDateEdit(settings);
	if (!time_edit)
		time_edit = createTimeEdit(settings);
#endif
	if (!program_choice)
		program_choice = createProgramChoice(settings, programs, dev);
}

BannSinglePuls *PageTermoReg::createHolidayWeekendBanner(SettingsPage *settings, QString icon, QString description)
{
	BannSinglePuls *bann = new BannSinglePuls(settings);
	bann->initBanner(bt_global::skin->getImage("forward"), icon, description);
	settings->appendBanner(bann);
	return bann;
}

PageSetDate *PageTermoReg::createDateEdit(SettingsPage *settings)
{
	PageSetDate *date_edit = new PageSetDate;
	connect(date_edit, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(date_edit, SIGNAL(dateSelected(QDate)), SLOT(dateSelected(QDate)));
	return date_edit;
}

PageSetDateTime *PageTermoReg::createDateTimeEdit(SettingsPage *settings)
{
	PageSetDateTime *date_time_edit = new PageSetDateTime(bt_global::skin->getImage("settings"), false);
	connect(date_time_edit, SIGNAL(Closed()), settings, SLOT(showPage()));
	connect(date_time_edit, SIGNAL(dateTimeSelected(QDate, BtTime)), SLOT(dateTimeSelected(QDate, BtTime)));
	return date_time_edit;
}

PageSetTime *PageTermoReg::createTimeEdit(SettingsPage *settings)
{
	PageSetTime *time_edit = new PageSetTime;
	connect(time_edit, SIGNAL(timeSelected(BtTime)), SLOT(timeSelected(BtTime)));
	connect(time_edit, SIGNAL(Closed()), SLOT(timeCancelled()));
	return time_edit;
}

WeeklyMenu *PageTermoReg::createProgramChoice(SettingsPage *settings, ProgramEntries programs, device *dev)
{
	WeeklyMenu *program_choice = new WeeklyMenu(programs);
	connect(program_choice, SIGNAL(programClicked(int)), SLOT(weekendHolidaySettingsEnd(int)));
	connect(program_choice, SIGNAL(Closed()), SLOT(programCancelled()));
	return program_choice;
}

void PageTermoReg::holidaySettingsStart()
{
	weekendHolidayStatus = HOLIDAY;
#ifdef LAYOUT_TS_10
	date_time_edit->setTitle(holiday_title);
	date_time_edit->showPage();
#else
	date_edit->showPage();
#endif
}

void PageTermoReg::weekendSettingsStart()
{
	weekendHolidayStatus = WEEKEND;
#ifdef LAYOUT_TS_10
	date_time_edit->setTitle(weekend_title);
	date_time_edit->showPage();
#else
	date_edit->showPage();
#endif
}

void PageTermoReg::dateSelected(QDate d)
{
	date_end = d;
	time_edit->showPage();
}

void PageTermoReg::dateTimeSelected(QDate d, BtTime t)
{
	date_end = d;
	time_end = t;
	program_choice->showPage();
}

void PageTermoReg::timeCancelled()
{
	date_edit->showPage();
}

void PageTermoReg::timeSelected(BtTime t)
{
	time_end = t;
	program_choice->showPage();
}

void PageTermoReg::programCancelled()
{
#ifdef LAYOUT_TS_10
	date_time_edit->showPage();
#else
	time_edit->showPage();
#endif
}

void PageTermoReg::weekendHolidaySettingsEnd(int program)
{
	if (weekendHolidayStatus == WEEKEND)
		dev()->setWeekendDateTime(date_end, time_end, program);
	else if (weekendHolidayStatus == HOLIDAY)
		dev()->setHolidayDateTime(date_end, time_end, program);
	else
		qWarning("PageTermoReg::weekendHolidaySettingsEnd: unknown status");
	showPage();
}


PageTermoReg4z::PageTermoReg4z(QDomNode n, ThermalDevice4Zones *device)
	: PageTermoReg(n)
{
	_dev = device;
	connect(_dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	createSettingsMenu(n);
}

ThermalDevice *PageTermoReg4z::dev()
{
	return _dev;
}

void PageTermoReg4z::createSettingsItem(QDomNode item, SettingsPage *settings, ThermalDevice4Zones *_dev)
{
	int id = getTextChild(item, "id").toInt();

	switch (id)
	{
	case BANNER_MANUAL_TIMED_MODE:
		timedManualSettings(item, settings, _dev);
		break;
	default:
		PageTermoReg::createSettingsItem(item, settings, _dev);
		break;
	}
}

void PageTermoReg4z::createSettingsMenu(QDomNode regulator_node)
{
	QDomNode n = getPageNodeFromChildNode(regulator_node, "h_lnk_pageID");
	SkinContext context(getTextChild(n, "cid").toInt());

	settings = new SettingsPage(n);
	connect(settings, SIGNAL(Closed()), SLOT(showPage()));

	// these do not have items
	createButtonsBanners(settings, _dev);

	foreach (const QDomNode &item, getChildren(n, "item"))
		createSettingsItem(item, settings, _dev);
}

void PageTermoReg4z::timedManualSettings(QDomNode n, SettingsPage *settings, ThermalDevice4Zones *dev)
{
	PageManualTimed *timed_manual_page = new PageManualTimed(dev, temp_scale, getTextChild(n, "descr"));
	timed_manual_page->setMaxHours(25);

	// timed manual banner
	BannSinglePuls *manual_timed = new BannSinglePuls(settings);
	manual_timed->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_manual_timed"),
				 getTextChild(n, "descr"));
	manual_timed->connectRightButton(timed_manual_page);
	settings->appendBanner(manual_timed);

	connect(manual_timed, SIGNAL(pageClosed()), settings, SLOT(showPage()));
	connect(timed_manual_page, SIGNAL(timeAndTempSelected(BtTime, int)), SLOT(manualTimedSelected(BtTime, int)));
}

void PageTermoReg4z::manualTimedSelected(BtTime time, int temp)
{
	_dev->setManualTempTimed(temp, time);
	showPage();
}


PageTermoReg99z::PageTermoReg99z(QDomNode n, ThermalDevice99Zones *device)
	: PageTermoReg(n)
{
	scenario_menu = 0;
	_dev = device;
	connect(_dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	createSettingsMenu(n);
}

ThermalDevice *PageTermoReg99z::dev()
{
	return _dev;
}

void PageTermoReg99z::setSeason(ThermalDevice::Season new_season)
{
	if (new_season == ThermalDevice::SE_SUMMER || new_season == ThermalDevice::SE_WINTER)
	{
		if (scenario_menu)
			scenario_menu->setSeason(new_season);
	}
	else
		qWarning("Received season is not SUMMER or WINTER, ignoring");
	PageTermoReg::setSeason(new_season);
}

void PageTermoReg99z::createSettingsItem(QDomNode item, SettingsPage *settings, ThermalDevice99Zones *_dev)
{
	int id = getTextChild(item, "id").toInt();

	switch (id)
	{
	case BANNER_SCENARIOS:
		scenarioSettings(item, settings, scenarios, _dev);
		break;
	default:
		PageTermoReg::createSettingsItem(item, settings, _dev);
		break;
	}
}

void PageTermoReg99z::createSettingsMenu(QDomNode regulator_node)
{
	QDomNode n = getPageNodeFromChildNode(regulator_node, "h_lnk_pageID");
	SkinContext context(getTextChild(n, "cid").toInt());

	settings = new SettingsPage(n);
	connect(settings, SIGNAL(Closed()), SLOT(showPage()));

	// these do not have items
	createButtonsBanners(settings, _dev);

	foreach (const QDomNode &item, getChildren(n, "item"))
		createSettingsItem(item, settings, _dev);
}

void PageTermoReg99z::scenarioSettings(QDomNode n, SettingsPage *settings, ProgramEntries scenarios, ThermalDevice99Zones *dev)
{
	scenario_menu = new ScenarioMenu(scenarios, getTextChild(n, "descr"));

	BannSinglePuls *scenario = new BannSinglePuls(settings);
	scenario->initBanner(bt_global::skin->getImage("forward"), bt_global::skin->getImage("regulator_scenario"),
			     getTextChild(n, "descr"));
	scenario->connectRightButton(scenario_menu);
	settings->appendBanner(scenario);

	connect(scenario, SIGNAL(pageClosed()), settings, SLOT(showPage()));
	connect(scenario_menu, SIGNAL(programClicked(int)), SLOT(scenarioSelected(int)));
}

void PageTermoReg99z::scenarioSelected(int scenario)
{
	_dev->setScenario(scenario);
	showPage();
}


ProgramMenu::ProgramMenu(ProgramEntries _descriptions, QString title)
{
	descriptions = _descriptions;
	buildPage(title);
}

void ProgramMenu::setSeason(ThermalDevice::Season new_season)
{
	if (new_season != season)
	{
		season = new_season;
		switch (season)
		{
		case ThermalDevice::SE_SUMMER:
			page_content->clear();
			createSummerBanners();
			// Because the drawContent must be called when the content has the
			// right size we call it only if the page is visible. The showEvent
			// callback is responsible to call the drawContent if the page is
			// hidden.
			if (isVisible())
				page_content->drawContent();
			break;
		case ThermalDevice::SE_WINTER:
			page_content->clear();
			createWinterBanners();
			if (isVisible())
				page_content->drawContent();
			break;
		}
	}
}

void ProgramMenu::createSeasonBanner(const QString season, const QString icon)
{
	for (int i = 0; i < descriptions.size(); ++i)
	{
		const QString &key = descriptions[i].first;
		const QString &value = descriptions[i].second;
		if (!key.startsWith(season))
			continue;

		int program_number = key.mid(season.length()).toInt();
		BannWeekly *bp = new BannWeekly(program_number);
		page_content->appendBanner(bp);
		connect(bp, SIGNAL(programNumber(int)), SIGNAL(programClicked(int)));
		bp->initBanner(bt_global::skin->getImage("ok"), icon, value);
	}
}

void ProgramMenu::createSummerBanners()
{
	createSeasonBanner(SUMMER_PREFIX, summer_icon);
}

void ProgramMenu::createWinterBanners()
{
	createSeasonBanner(WINTER_PREFIX, winter_icon);
}


WeeklyMenu::WeeklyMenu(ProgramEntries programs, QString title) : ProgramMenu(programs, title)
{
	summer_icon = bt_global::skin->getImage("summer_program");
	winter_icon = bt_global::skin->getImage("winter_program");
	season = ThermalDevice::SE_SUMMER;
	createSummerBanners();
}


ScenarioMenu::ScenarioMenu(ProgramEntries scenarios, QString title) : ProgramMenu(scenarios, title)
{
	summer_icon = bt_global::skin->getImage("summer_scenario");
	winter_icon = bt_global::skin->getImage("winter_scenario");
	season = ThermalDevice::SE_SUMMER;
	createSummerBanners();
}

