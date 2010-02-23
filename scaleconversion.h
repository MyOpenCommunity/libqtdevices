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

/**
 * Creates a string from an integer that represents Celsius degrees.
 * \param celsius_temp The temperature to be converted.
 * \return A string with the temperature.
 */
QString celsiusString(int celsius_temp);

/**
 * Creates a string from an integer that represents Fahrenheit degrees.
 * \param fahr_temp The temperature to be converted.
 * \return A string with the temperature.
 */
QString fahrenheitString(int fahr_temp);

/**
 * Convert celsius degrees to fahrenheit
 */
float toFahrenheit(float temperature);
float toCelsius(float temperature);

/**
 * Convert a temperature in BTicino 4-digit format to a signed integer in Celsius degrees.
 * Bticino 4-digit format is as follows:
 * sDDd
 * - s: the sign digit, 1 means negative.
 * - DD: the integral part of the temperature.
 * - d: the decimal part of the temperature.
 * For example, 1235 (that means -23.5 Celsius degrees) will be transformed as -235.
 * \param bt_temp A temperature in BTicino 4-digit format.
 * \return A temperature in Celsius degrees, with precision of 1/10 of degree.
 */
int bt2Celsius(unsigned bt_temp);

/**
 * Convert a temperature in BTicino 4-digit format to a signed integer in Fahrenheit degrees.
 * This is very similar to bt2Celsius. This function uses truncation so it may have rounding
 * problems.
 * \param bt_temp A temperature in BTicino 4-digit format
 * \return A temperature in Fahrenheit degrees, with precision of 1/10 of degree.
 */
int bt2Fahrenheit(unsigned bt_temp);

/**
 * Convert a temperature in a signed integer with precision of 1/10 of degree in BTicino 4-digit
 * format.
 * For example, -134 (that represent -13.4 Celsius) become 1134 in BTicino 4-digit format.
 * \param celsius_temp A temperature in Celsius degrees where unit digit represent 1/10 of degree.
 * \return A BTicino 4-digit format temperature.
 */
unsigned celsius2Bt(int celsius_temp);

/**
 * Convert a temperature in Fahrenheit to BTicino 4-digit format. It is very similar to celsius2Bt.
 * Be carefull that this function is lossy, ie. uses rounding for its computation.
 * \param fahr_temp A temperature in Fahrenheit degrees.
 * \return A BTicino 4-digit format temperature.
 */
unsigned fahrenheit2Bt(int fahr_temp);
#endif // SCALECONVERSION_H
