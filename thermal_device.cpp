#include "thermal_device.h"
#include "openmsg.h"
#include "bttime.h"
#include "generic_functions.h" // createWriteRequestOpen

#include <QDebug>


// ThermalDevice implementation

const QString ThermalDevice::WHO = "4";

ThermalDevice::ThermalDevice(QString where)
	: device(QString(WHO), QString("#") + where, false, -1)
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
		sl[DIM_STATUS] = QVariant(ST_PROTECTION);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case SUM_OFF:
		sl[DIM_STATUS] = QVariant(ST_OFF);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case SUM_MANUAL:
	case SUM_MANUAL_TIMED:
		{
			unsigned arg_count = msg.whatArgCnt();
			if (arg_count < 1)
			{
				qDebug("manual frame (%s), no what args found!!! About to crash...", msg.frame_open);
			}
			int sp = msg.whatArgN(0);
			sl[DIM_TEMPERATURE] = QVariant(sp);
		}
		sl[DIM_STATUS] = QVariant(command == SUM_MANUAL ? ST_MANUAL : ST_MANUAL_TIMED);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case SUM_WEEKEND:
		sl[DIM_STATUS] = QVariant(ST_WEEKEND);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case SUM_PROGRAM:
		sl[DIM_PROGRAM] = QVariant(program);
		sl[DIM_STATUS] = QVariant(ST_PROGRAM);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case SUM_SCENARIO:
		sl[DIM_SCENARIO] = QVariant(program);
		sl[DIM_STATUS] = QVariant(ST_SCENARIO);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case SUM_HOLIDAY:
		sl[DIM_STATUS] = QVariant(ST_HOLIDAY);
		sl[DIM_SEASON] = QVariant(SE_SUMMER);
		break;

	case WIN_PROTECTION:
		sl[DIM_STATUS] = QVariant(ST_PROTECTION);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
		break;

	case WIN_OFF:
		sl[DIM_STATUS] = QVariant(ST_OFF);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
		break;

	case WIN_MANUAL:
	case WIN_MANUAL_TIMED:
		{
			unsigned arg_count = msg.whatArgCnt();
			if (arg_count < 1)
			{
				qDebug("manual frame (%s), no what args found!!! About to crash...", msg.frame_open);
			}
			int sp = msg.whatArgN(0);
			sl[DIM_TEMPERATURE] = QVariant(sp);
		}
		sl[DIM_STATUS] = QVariant(command == WIN_MANUAL ? ST_MANUAL : ST_MANUAL_TIMED);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
		break;

	case WIN_WEEKEND:
		sl[DIM_STATUS] = QVariant(ST_WEEKEND);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
		break;

	case WIN_PROGRAM:
		sl[DIM_PROGRAM] = QVariant(program);
		sl[DIM_STATUS] = QVariant(ST_PROGRAM);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
		break;

	case WIN_SCENARIO:
		sl[DIM_SCENARIO] = QVariant(program);
		sl[DIM_STATUS] = QVariant(ST_SCENARIO);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
		break;

	case WIN_HOLIDAY:
		sl[DIM_STATUS] = QVariant(ST_HOLIDAY);
		sl[DIM_SEASON] = QVariant(SE_WINTER);
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
