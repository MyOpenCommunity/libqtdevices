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


#ifndef SCALECONVERSION_H
#define SCALECONVERSION_H

#include <QString>

/*!
	\ingroup ThermalRegulation
	\brief Format a Celsius temperature with precision of 1/10 of degree using dot as decimal separator (es. 45 -> "4.5C").
 */
QString celsiusString(int celsius_temp);

/*!
	\ingroup ThermalRegulation
	\brief Format a Fahrenheit temperature with precision of 1/10 of degree using dot as decimal separator (es. 48 -> "4.8F").
 */
QString fahrenheitString(int fahr_temp);

/*!
	\ingroup ThermalRegulation
	\brief Convert Celsius degrees to Fahrenheit.
 */
float toFahrenheit(float temperature);

/*!
	\ingroup ThermalRegulation
	\brief Convert Fahrenheit degrees to Celsius.
 */
float toCelsius(float temperature);

/*!
	\ingroup ThermalRegulation
	\brief  Convert a temperature in BTicino 4-digit format to a signed integer with precision of 1/10 of Celsius degree.

	Bticino 4-digit format is \c sDDd
	- s: the sign digit, 1 means negative.
	- DD: the integral part of the temperature.
	- d: the decimal part of the temperature.

	For example, 1235 (that means -23.5 Celsius degrees) will be transformed as -235.

	\see bt2Fahrenheit()
	\see celsius2Bt()
	\see fahrenheit2Bt()
 */
int bt2Celsius(unsigned bt_temp);

/*!
	\ingroup ThermalRegulation
	\brief Convert a temperature in BTicino 4-digit format to a signed integer with precision of 1/10 of Fahrenheit degree.

	There might be precision loss because of the Celsius -> Fahrenheit conversion.

	\see bt2Celsius()
	\see celsius2Bt()
	\see fahrenheit2Bt()
 */
int bt2Fahrenheit(unsigned bt_temp);

/*!
	\ingroup ThermalRegulation
	\brief Convert a temperature in a signed integer with precision of 1/10 of degree in BTicino 4-digit format.

	For example, -134 (that represent -13.4 Celsius) becomes 1134 in BTicino 4-digit format.

	\see bt2Celsius()
	\see bt2Fahrenheit()
	\see fahrenheit2Bt()
 */
unsigned celsius2Bt(int celsius_temp);

/*!
	\ingroup ThermalRegulation
	\brief Convert a temperature in Fahrenheit to BTicino 4-digit format.

	There might be precision loss because of the Celsius -> Fahrenheit conversion.

	\see bt2Celsius()
	\see bt2Fahrenheit()
	\see celsius2Bt()
 */
unsigned fahrenheit2Bt(int fahr_temp);

#endif // SCALECONVERSION_H
