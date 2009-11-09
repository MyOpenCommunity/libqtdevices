#include "thermal_device.h"
#include "openmsg.h"
#include "bttime.h"
#include "generic_functions.h" // createWriteRequestOpen

#include <QDebug>

enum what_t
{
	SUMMER = 0,
	WINTER = 1,
	GENERIC_MODE = 3,
	TEMPERATURE_SET = 14,            // set the temperature in manual operation, this is a dimension
	HOLIDAY_DATE_END = 30,           // set the end date of holiday mode, this is a dimension (grandezza)
	HOLIDAY_TIME_END = 31,           // set the end time of holiday mode, this is a dimension (grandezza)
	MANUAL_TIMED_END = 32,           // set the end time of timed manual mode

	// summer specific identifiers
	SUM_PROTECTION = 202,            // protection
	SUM_OFF = 203,                   // off
	SUM_MANUAL = 210,                // manual operation (all zones in setpoint temperature)
	SUM_MANUAL_TIMED = 212,          // manual operation (24h maximum)
	SUM_WEEKEND = 215,               // weekend operation (festivo)
	SUM_PROGRAM = 2100,              // weekly program (1 out of 3)
	SUM_SCENARIO = 2200,             // scenario (1 out of 16, 99zones thermal regulator only)
	SUM_HOLIDAY = 23000,             // holiday operation (programma ferie)

	// winter specific identifiers
	WIN_PROTECTION = 102,
	WIN_OFF = 103,                   // off
	WIN_MANUAL = 110,                // manual operation (all zones in setpoint temperature)
	WIN_MANUAL_TIMED = 112,          // manual operation (24h maximum)
	WIN_WEEKEND = 115,               // weekend operation (festivo)
	WIN_PROGRAM = 1100,              // weekly program (1 out of 3)
	WIN_SCENARIO = 1200,             // scenario (1 out of 16, 99zones thermal regulator only)
	WIN_HOLIDAY = 13000,             // holiday operation (programma ferie)

	// generic identifiers (useful for issuing commands)
	GENERIC_PROTECTION = 302,
	GENERIC_OFF = 303,
	GENERIC_MANUAL_TIMED = 312,      // timed manual operation (generic mode)
	GENERIC_WEEKEND = 315,           // command to set weekend mode
	WEEK_PROGRAM = 3100,             // command to set the program to be executed (generic mode)
	// remember to add the program number to this number
	SCENARIO_PROGRAM = 3200,         // command to set the scenario to be executed (generic mode)
	// remember to add the program number to this number
	HOLIDAY_NUM_DAYS = 33000,        // command to set the number of days of holiday mode (generic mode)
	// remember to add the number of days to this number
};


// ThermalDevice implementation

ThermalDevice::ThermalDevice(QString where)
	: device(QString("4"), QString("#") + where, false, -1)
{
}

void ThermalDevice::sendWriteRequest(const QString &what)
{
	sendFrame(createWriteRequestOpen(who, what, where));
}

void ThermalDevice::setOff()
{
	sendCommand(QString::number(GENERIC_OFF));
}

void ThermalDevice::setProtection()
{
	sendCommand(QString::number(GENERIC_PROTECTION));
}

void ThermalDevice::setSummer()
{
	sendCommand(QString::number(SUMMER));
}

void ThermalDevice::setWinter()
{
	sendCommand(QString::number(WINTER));
}

void ThermalDevice::setManualTemp(unsigned temperature)
{
	// sanity check on input
	if (temperature > 1000)
	{
		unsigned tmp = temperature - 1000;
		if (tmp < 50 || tmp > 400)
			return;
	}

	QString what;
	// temperature is 4 digits wide
	// prepend some 0 if temperature is positive
	what.sprintf("%d*%04d*%d", TEMPERATURE_SET, temperature, GENERIC_MODE);

	sendWriteRequest(what);
}

void ThermalDevice::setWeekProgram(int program)
{
	sendCommand(QString::number(WEEK_PROGRAM + program));
}

void ThermalDevice::setWeekendDateTime(QDate date, BtTime time, int program)
{
	// we need to send 3 frames
	// - frame at par. 2.3.9, to set what program has to be executed at the end of weekend mode
	// - frame at par. 2.3.16 to set date
	// - frame at par. 2.3.17 to set time
	//
	// First frame: set program
	sendCommand(QString::number(GENERIC_WEEKEND) + "#" + QString::number(WEEK_PROGRAM + program));

	// Second frame: set date
	setHolidayEndDate(date);

	// Third frame: set time
	setHolidayEndTime(time);
}

void ThermalDevice::setHolidayDateTime(QDate date, BtTime time, int program)
{
	// we need to send 3 frames, as written in bug #44
	// - frame at par. 2.3.10, with number_of_days = 2 (dummy number, we set end date and time explicitly with
	// next frames), to set what program has to be executed at the end of holiday mode
	// - frame at par. 2.3.16 to set date
	// - frame at par. 2.3.17 to set time
	//
	// First frame: set program
	const int number_of_days = 2;
	sendCommand(QString("%1#%2").arg(HOLIDAY_NUM_DAYS + number_of_days).arg(WEEK_PROGRAM + program));

	// Second frame: set date
	setHolidayEndDate(date);

	// Third frame: set time
	setHolidayEndTime(time);
}

void ThermalDevice::setHolidayEndDate(QDate date)
{
	QString what;
	// day and month must be padded with 0 if they have only one digit
	what.sprintf("%d*%02d*%02d*%d", HOLIDAY_DATE_END, date.day(), date.month(), date.year());

	sendWriteRequest(what);
}

void ThermalDevice::setHolidayEndTime(BtTime time)
{
	QString what;
	// hours and minutes must be padded with 0 if they have only one digit
	what.sprintf("%d*%02d*%02d", HOLIDAY_TIME_END, time.hour(), time.minute());

	sendWriteRequest(what);
}

unsigned ThermalDevice::minimumTemp() const
{
	return 30;
}

void ThermalDevice::manageFrame(OpenMsg &msg)
{
	if (where.toStdString() != msg.whereFull())
		return;

	int what = msg.what();
	int command = commandRange(what);
	int program = what - command;
	StatusList sl;

	qDebug() << "ThermalDevice command" << command << "program" << program;

	switch (command)
	{
	case 21: // remote control enabled
	case 30: // malfunctioning found
	case 31: // battery ko
		break;

	case SUM_PROTECTION:
		sl[DIM_STATUS] = ST_PROTECTION;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case SUM_OFF:
		sl[DIM_STATUS] = ST_OFF;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case SUM_MANUAL:
	case SUM_MANUAL_TIMED:
		{
			Q_ASSERT_X(msg.whatArgCnt() > 0, "ThermalDevice::manageFrame", "Manual setting frame with no arguments received");
			int sp = msg.whatArgN(0);
			sl[DIM_TEMPERATURE] = sp;
		}
		sl[DIM_STATUS] = command == SUM_MANUAL ? ST_MANUAL : ST_MANUAL_TIMED;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case SUM_WEEKEND:
		sl[DIM_STATUS] = ST_WEEKEND;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case SUM_PROGRAM:
		sl[DIM_PROGRAM] = program;
		sl[DIM_STATUS] = ST_PROGRAM;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case SUM_SCENARIO:
		sl[DIM_SCENARIO] = program;
		sl[DIM_STATUS] = ST_SCENARIO;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case SUM_HOLIDAY:
		sl[DIM_STATUS] = ST_HOLIDAY;
		sl[DIM_SEASON] = SE_SUMMER;
		break;

	case WIN_PROTECTION:
		sl[DIM_STATUS] = ST_PROTECTION;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	case WIN_OFF:
		sl[DIM_STATUS] = ST_OFF;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	case WIN_MANUAL:
	case WIN_MANUAL_TIMED:
		{
			Q_ASSERT_X(msg.whatArgCnt() > 0, "ThermalDevice::manageFrame", "Manual setting frame with no arguments received");
			int sp = msg.whatArgN(0);
			sl[DIM_TEMPERATURE] = sp;
		}
		sl[DIM_STATUS] = command == WIN_MANUAL ? ST_MANUAL : ST_MANUAL_TIMED;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	case WIN_WEEKEND:
		sl[DIM_STATUS] = ST_WEEKEND;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	case WIN_PROGRAM:
		sl[DIM_PROGRAM] = program;
		sl[DIM_STATUS] = ST_PROGRAM;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	case WIN_SCENARIO:
		sl[DIM_SCENARIO] = program;
		sl[DIM_STATUS] = ST_SCENARIO;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	case WIN_HOLIDAY:
		sl[DIM_STATUS] = ST_HOLIDAY;
		sl[DIM_SEASON] = SE_WINTER;
		break;

	default:
		break;
	}

	if (sl.count() > 0)
		emit status_changed(sl);
}

int ThermalDevice::commandRange(int what)
{
	if (what > 10000)
		return (what/1000) * 1000;
	else if (what > 1000)
		return (what/100) * 100;
	else
		return what;
}


// ThermalDevice4Zones implementation

ThermalDevice4Zones::ThermalDevice4Zones(QString where)
	: ThermalDevice(where)
{
}

void ThermalDevice4Zones::setManualTempTimed(int temperature, BtTime time)
{
	// we need to send 2 frames
	// - frame at par. 2.3.13, with number_of_hours = 2 (dummy number, we set end time explicitly with
	// next frame), to set the temperature of timed manual operation
	// - frame at par. 2.3.18 to set hours and minutes of mode end. Timed manual operation can't last longer
	// than 24 hours.
	//
	// First frame: set temperature
	QString what;
	what.sprintf("%d#%04d#2", GENERIC_MANUAL_TIMED, temperature);

	sendCommand(what);

	// Second frame: set end time
	QString what2;
	what2.sprintf("%d*%02d*%02d", MANUAL_TIMED_END, time.hour(), time.minute());

	sendWriteRequest(what2);
}

unsigned ThermalDevice4Zones::maximumTemp() const
{
	return 350;
}

thermo_type_t ThermalDevice4Zones::type() const
{
	return THERMO_Z4;
}


// ThermalDevice99Zones implementation

ThermalDevice99Zones::ThermalDevice99Zones(QString where)
	: ThermalDevice(where)
{
}

void ThermalDevice99Zones::setScenario(int scenario)
{
	sendCommand(QString::number(SCENARIO_PROGRAM + scenario));
}

unsigned ThermalDevice99Zones::maximumTemp() const
{
	return 400;
}

thermo_type_t ThermalDevice99Zones::type() const
{
	return THERMO_Z99;
}
