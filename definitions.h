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


#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <QHash>


#ifdef BT_HARDWARE_DM365

// The configuration file for BTouch
#define MY_FILE_USER_CFG_DEFAULT "/var/tmp/conf.xml"

#else

// The configuration file for BTouch
#define MY_FILE_USER_CFG_DEFAULT "cfg/conf.xml"

// The file to generate when changing the configuration file to avoid being resetted from bt_processi
#define FILE_CHANGE_CONF "BTOUCH_CHANGE_CONF"

#endif

// The file name to create for software watchdog
#define FILE_WDT "/var/tmp/bticino/bt_wd/BtExperience_qws"

// The delay to use in the starting phase to avoid bottlenecks in the scs bus.
#define TS_NUMBER_FRAME_DELAY 500

// The path for sounds
#define SOUND_PATH "cfg/extra/10/"

/**
 * The following enum defines the keys of the global configuration.
 */
enum GlobalField
{
	LANGUAGE,
	TEMPERATURE_SCALE,
	DATE_FORMAT,
	MODEL,
	NAME,
	PI_ADDRESS,
	PI_MODE,
	GUARD_UNIT_ADDRESS,
	AMPLIFIER_ADDRESS,
	SOURCE_ADDRESS,
	TS_NUMBER,
	INIT_COMPLETE,
	DEFAULT_PE,
	USER_PASSWORD,
	USER_PASSWORD_ENABLED,
};

enum DateFormat
{
	EUROPEAN_DATE = 0,   // dd.mm.yy
	USA_DATE = 1,        // mm.dd.yy
	YEAR_FIRST = 2,      // yy.mm.dd
};

enum TemperatureScale
{
	CELSIUS = 0,
	FAHRENHEIT,
};

namespace bt_global {
	/// a global object to store global configuration parameters
	extern QHash<GlobalField, QString> *config;
}

#endif //_DEFINITIONS_H_


