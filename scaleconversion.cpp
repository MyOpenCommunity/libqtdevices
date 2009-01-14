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

#include <cmath>

QString celsiusString(int celsius_temp)
{
	QString str;
	str.sprintf("%d", celsius_temp);
	if((celsius_temp < 10) && (celsius_temp > -10))
		str.insert(str.length() -1, "0.");
	else
		str.insert(str.length() -1, ".");
	str += TEMP_DEGREES"C";
	return str;
}

QString fahrenheitString(int fahr_temp)
{
	QString str;
	str.sprintf("%d", fahr_temp);
	str.insert(str.length() -1, ".");
	str += TEMP_DEGREES"F";
	return str;
}

float toFahrenheit(float temperature)
{
	return ((temperature * 9. / 5.) + 32);
}

float toCelsius(float temperature)
{
	return ((temperature - 32) * 5. / 9.);
}

int bt2Celsius(unsigned bt_temp)
{
	bool isNegative = false;
	if (bt_temp >= 1000)
	{
		isNegative = true;
		bt_temp -= 1000;
	}
	int return_value = static_cast<int>(bt_temp);
	return (isNegative ? -return_value : return_value);
}

int bt2Fahrenheit(unsigned bt_temp)
{
	bool isNegative = false;
	if (bt_temp > 1000)
	{
		isNegative = true;
		bt_temp -= 1000;
	}
	// conversion
	float fahr = bt_temp;
	if (isNegative)
		fahr = -fahr;
	fahr /= 10;
	fahr = toFahrenheit(fahr);
	return static_cast<int>(round(fahr * 10));
}

unsigned celsius2Bt(int celsius_temp)
{
	unsigned bt_temp = 0;
	if (celsius_temp < 0)
	{
		bt_temp += 1000;
		celsius_temp = -celsius_temp;
	}
	bt_temp += static_cast<unsigned>(celsius_temp);
	return bt_temp;
}

unsigned fahrenheit2Bt(int fahr_temp)
{
	// use floats to convert to celsius
	float tmp = fahr_temp;
	tmp /= 10;
	tmp = toCelsius(tmp);
	// now create bticino form
	int celsius_temp = static_cast<int>(round(tmp * 10));
	return celsius2Bt(celsius_temp);
}
