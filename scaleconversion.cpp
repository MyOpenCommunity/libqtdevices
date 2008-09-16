/*!
 * \scaleconversion.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A set of functions to convert between Celsius and Fahrenheit scales
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "scaleconversion.h"
#include "main.h"

QString celsiusString(unsigned temperature)
{
	float icx = temperature;
	QString qtemp = "";
	char tmp[10];
	if (icx >= 1000)
	{
		icx = icx - 1000;
		qtemp = "-";
	}
	icx /= 10;
	sprintf(tmp, "%.1f", icx);
	qtemp += tmp;
	qtemp += TEMP_DEGREES"C";
	return qtemp;
}

QString fahrenheitString(unsigned temperature)
{
	bool isNegative = false;
	if (temperature > 1000)
	{
		isNegative = true;
		temperature -= 1000;
	}
	float fahr = temperature;
	if (isNegative)
		fahr = -fahr;
	fahr = toFahrenheit(fahr / 10);
	char tmp[15];
	// conversion to string
	snprintf(tmp, 15, "%.1f", fahr);

	QString temp;
	temp = tmp;
	temp += TEMP_DEGREES"F";
	return temp;
}

QString convertFahrenheitToString(unsigned temperature)
{
	float fahr = temperature;
	fahr /= 10;
	char tmp[15];
	// conversion to string
	snprintf(tmp, 15, "%.1f", fahr);

	QString temp;
	temp = tmp;
	temp += TEMP_DEGREES"F";
	return temp;
}

float toFahrenheit(float temperature)
{
	return ((temperature * 9. / 5.) + 32);
}

float toCelsius(float temperature)
{
	return ((temperature - 32) * 5. / 9.);
}

unsigned toCelsius(unsigned temperature)
{
	float tmp = temperature;
	tmp = toCelsius(tmp / 10);
	unsigned new_temperature = 0;
	if (tmp < 0)
	{
		new_temperature = 1000;
		tmp = -tmp;
	}
	new_temperature += static_cast<unsigned>(tmp * 10);
	return new_temperature;
}

unsigned toFahrenheit(unsigned temperature)
{
	float tmp = temperature;
	tmp /= 10;
	return static_cast<unsigned>(toFahrenheit(tmp) * 10);
}

