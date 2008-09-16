/*!
 * \scaleconversion.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A set of functions to convert between Celsius and Fahrenheit scales
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef SCALECONVERSION_H
#define SCALECONVERSION_H

#include <qstring.h>

/**
 * Creates a string to visualize the temperature.
 * \param temperature The temperature in BTicino form, Celsius degrees.
 * \return A string with that rapresents the temperature in Celsius degrees.
 */
QString celsiusString(unsigned temperature);

/**
 * Creates a string to visualize the temperature.
 * \param temperature The temperature in BTicino form, Celsius degrees.
 * \param needsConversionToFahrenheit True if temperature is in Celsius and must be converted to Fahrenheit
 * \return A string with that rapresents the temperature in Fahrenheit degrees.
 */
QString fahrenheitString(unsigned temperature);

/**
 * Creates a string from an unsigned in fahrenheit degrees.
 * \param temperature The temperature in Fahrenheit degrees, in 1/10 of degrees. Be careful: we can't use BTicino form
 * because we can go up to 104 fahr degrees, which overflows BTicino representation.
 * \return A string with that rapresents the temperature in Fahrenheit degrees.
 */
QString convertFahrenheitToString(unsigned temperature);

/**
 * Convert celsius degrees to fahrenheit
 */
float toFahrenheit(float temperature);
float toCelsius(float temperature);
/**
 * Convert fahrenheit to celsius. Convert to BTicino 4-digit format if needed.
 * \param temperature A fahrenheit temperature in 1/10 of degrees (ie. 99.4 -> 994)
 * \return A temperature in BTicino 4-digit format
 */
unsigned toCelsius(unsigned temperature);
/**
 * Convert celsius to fahrenheit.
 * \param temperature A celsius temperature in BTicino 4-digit format
 * \return A temperature in fahrenheit degrees. Watch out that this CAN'T be expressed in 4-digit format
 * since 104 degrees (-> 40 Celsius) is meaningful.
 */
unsigned toFahrenheit(unsigned temperature);
#endif // SCALECONVERSION_H
