#include "thermal_device.h"
#include "bttime.h"


// ThermalDevice implementation

ThermalDevice::ThermalDevice(QString where, bool p, int g)
	: device(QString("4"), where, p, g)
{
}

void ThermalDevice::setOff()
{
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(GENERIC_OFF)).arg(QString("#") + where);
	sendFrame(msg);
}

void ThermalDevice::setProtection()
{
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(GENERIC_PROTECTION)).arg(QString("#") + where);
	sendFrame(msg);
}

void ThermalDevice::setSummer()
{
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(SUMMER)).arg(QString("#") + where);
	sendFrame(msg);
}

void ThermalDevice::setWinter()
{
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(WINTER)).arg(QString("#") + where);
	sendFrame(msg);
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

	const QString sharp_where = QString("#") + where;
	QString what;
	// temperature is 4 digits wide
	// prepend some 0 if temperature is positive
	what.sprintf("#%d*%04d*%d", TEMPERATURE_SET, temperature, GENERIC_MODE);

	QString msg = QString("*#") + who + "*" + sharp_where + "*" + what + "##";
	sendFrame(msg);
}

void ThermalDevice::setWeekProgram(int program)
{
	const QString what = QString::number(WEEK_PROGRAM + program);
	const QString sharp_where = QString("#") + where;
	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	sendFrame(msg);
}

void ThermalDevice::setWeekendDateTime(QDate date, BtTime time, int program)
{
	const QString sharp_where = QString("#") + where;

	// we need to send 3 frames
	// - frame at par. 2.3.9, to set what program has to be executed at the end of weekend mode
	// - frame at par. 2.3.16 to set date
	// - frame at par. 2.3.17 to set time
	//
	// First frame: set program
	const int what_program = WEEK_PROGRAM + program;
	QString what = QString::number(GENERIC_WEEKEND) + "#" + QString::number(what_program);

	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	sendFrame(msg);

	// Second frame: set date
	setHolidayEndDate(date);

	// Third frame: set time
	setHolidayEndTime(time);
}

void ThermalDevice::setHolidayDateTime(QDate date, BtTime time, int program)
{
	const QString sharp_where = QString("#") + where;

	// we need to send 3 frames, as written in bug #44
	// - frame at par. 2.3.10, with number_of_days = 2 (dummy number, we set end date and time explicitly with
	// next frames), to set what program has to be executed at the end of holiday mode
	// - frame at par. 2.3.16 to set date
	// - frame at par. 2.3.17 to set time
	//
	// First frame: set program
	const int number_of_days = 2;
	QString what;
	what.sprintf("%d#%d", HOLIDAY_NUM_DAYS + number_of_days, WEEK_PROGRAM + program);

	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	sendFrame(msg);

	// Second frame: set date
	setHolidayEndDate(date);

	// Third frame: set time
	setHolidayEndTime(time);
}

void ThermalDevice::setHolidayEndDate(QDate date)
{
	const QString sharp_where = QString("#") + where;

	QString what;
	// day and month must be padded with 0 if they have only one digit
	what.sprintf("#%d*%02d*%02d*%d", HOLIDAY_DATE_END, date.day(), date.month(), date.year());

	QString msg = QString("*#") + who + "*" + sharp_where + "*" + what + "##";
	sendFrame(msg);
}

void ThermalDevice::setHolidayEndTime(BtTime time)
{
	const QString sharp_where = QString("#") + where;

	QString what;
	// hours and minutes must be padded with 0 if they have only one digit
	what.sprintf("#%d*%02d*%02d", HOLIDAY_TIME_END, time.hour(), time.minute());

	QString msg = QString("*#") + who + "*" + sharp_where + "*" + what + "##";
	sendFrame(msg);
}

unsigned ThermalDevice::minimumTemp() const
{
	return 30;
}


// ThermalDevice4Zones implementation

ThermalDevice4Zones::ThermalDevice4Zones(QString where, bool p, int g)
	: ThermalDevice(where, p, g)
{
	stat.append(new device_status_thermal_regulator_4z());
	// setup_frame_interpreter(new frame_interpreter_thermal_regulator(who, where, p, g));
}

void ThermalDevice4Zones::setManualTempTimed(int temperature, BtTime time)
{
	const QString sharp_where = QString("#") + where;

	// we need to send 2 frames
	// - frame at par. 2.3.13, with number_of_hours = 2 (dummy number, we set end time explicitly with
	// next frame), to set the temperature of timed manual operation
	// - frame at par. 2.3.18 to set hours and minutes of mode end. Timed manual operation can't last longer
	// than 24 hours.
	//
	// First frame: set temperature
	const char* number_of_hours = "2";
	QString what;
	what.sprintf("%d#%04d#%s", GENERIC_MANUAL_TIMED, temperature, number_of_hours);

	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	sendFrame(msg);

	// Second frame: set end time
	QString what2;
	what2.sprintf("#%d*%02d*%02d", MANUAL_TIMED_END, time.hour(), time.minute());

	msg = QString("*#") + who + "*" + sharp_where + "*" + what2 + "##";
	sendFrame(msg);
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

ThermalDevice99Zones::ThermalDevice99Zones(QString where, bool p, int g)
	: ThermalDevice(where, p, g)
{
	stat.append(new device_status_thermal_regulator_99z());
	// setup_frame_interpreter(new frame_interpreter_thermal_regulator(who, where, p, g));
}

void ThermalDevice99Zones::setScenario(int scenario)
{
	const QString what = QString::number(SCENARIO_PROGRAM + scenario);
	const QString sharp_where = QString("#") + where;
	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	sendFrame(msg);
}

unsigned ThermalDevice99Zones::maximumTemp() const
{
	return 400;
}

thermo_type_t ThermalDevice99Zones::type() const
{
	return THERMO_Z99;
}
