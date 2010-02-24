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


#ifndef __DEVICE_STATUS_H__
#define __DEVICE_STATUS_H__

#include <QList>


//! State variable
class stat_var
{
public:
	enum type
	{
		NONE = -1,
		SPEED = 0,
		LEV ,
		OLD_LEV,
		TEMP ,
		ON_OFF ,
		STAT,    // Device dependent status
		HH,
		MM,
		SS,
		TEMPERATURE,
		AUDIO_LEVEL,
		PENDING_CALL,
		FREQ,
		STAZ,
		RDS0,
		RDS1,
		RDS2,
		RDS3,
		RDS4,
		RDS5,
		RDS6,
		RDS7,
		LOCAL,   // Temperature probe personal setpoint delta
		SP,      // Temperature probe setpoint
		ACTIVE_SOURCE,
		FAULT,
		CRONO,   // Secret, who knows?
		INFO_CENTRALE,
		FANCOIL_SPEED,
		INFO_MCI,
		WORD,      //0-65535
		PROGRAM,   // weekly program currently set in the thermal regulator
		SCENARIO,  // scenario currentrly set in thermal regulator (99 zones only)
		SEASON,    // summer / winter
		THERMR,    // status of thermal regulation (off, protection, manual, week program, scenario, holiday, weekend)
	};
private:
	type t;
	int val;
	int min;
	int max;
	int step;
	//! True when initialized
	bool _initialized;
public:
	stat_var(stat_var::type _t, int val, int min, int max, 
			int step, bool initialized = false);
	stat_var(stat_var::type=NONE);
	void get_val(int&);
	int get_val() const;
	void set_val(int&);
	void get_min(int&);
	void set_min(int&);
	void get_max(int&);
	void set_max(int&);
	void get_step(int&);
	int get_step();
	void set_step(int&);
	bool initialized();
	//! Invalidate variable contents
	void invalidate();
	//! Force initialized state
	void force_initialized();
	~stat_var();
};

//! Device status description
class device_status 
{
public:
	typedef enum
	{
		LIGHTS = 0,
		DIMMER ,
		DIMMER100 ,
		NEWTIMED,
		AUTOM,
		AMPLIFIER,
		RADIO,
		DOORPHONE,
		IMPANTI,
		ZONANTI,
		MODSCEN,
		SOUNDMATR,
 		SUPERVISION_MCI,
		THERMAL_REGULATOR_4Z,
		THERMAL_REGULATOR_99Z,
	} type;
private:
	type t;
	QList<stat_var*> vars;
	//! True when initialized
	bool _initialized;
	//! True when init requested
	bool _init_requested;
public:
	device_status(device_status::type);
	//! Read type
	device_status::type get_type();
	//! Set type
	void set_type(device_status::type& t);
	void add_var(int index, stat_var *v);
	int read(int index, stat_var& out);
	int write_val(int index, stat_var& val);
	//! Returns true when status initialized
	bool initialized();
	//! Returns true when status initialize has already been requested
	bool init_requested();
	//! Sets init_requested flag
	void mark_init_requested();
	void invalidate();
	//! Force initialized status
	void force_initialized();
	//! Return delay for init request deferral (msecs)
	virtual int init_request_delay() { return 0; }
	virtual ~device_status();
};

#ifndef LIGHT_REQ_DELAY
#define LIGHT_REQ_DELAY 1500
#endif


//! SUPERVISION MCI status
class device_status_mci : public device_status {
 private:
 public:
   enum {
     MCI_STATUS_INDEX = 0,
     MCI_AUTOTEST_FREQ_INDEX = 1,
   } ind;
   device_status_mci(); 
   //~device_status_mci();
	 bool GetMciStatusBit(int bit);
	 int GetMciAutotestFreq();
	 bool SetMciStatus(int s);
	 bool SetMciAutotestFreq(int s);
};

//! Simple light status
class device_status_light : public device_status
{
private:
public:
	enum
	{
		ON_OFF_INDEX = 0,
	} ind;
	device_status_light(); //{ add_var(new stat_var(ON_OFF, 0, 1, 1)); } ;
	virtual int init_request_delay() { return LIGHT_REQ_DELAY; }
	//~device_status_light();
};

#ifndef DIMMER_REQ_DELAY
#define DIMMER_REQ_DELAY 4000
#endif

//! Dimmer status
class device_status_dimmer : public device_status {
private:
public:
	enum
	{
		LEV_INDEX = 0,
		OLD_LEV_INDEX,
		FAULT_INDEX,
	} ind;
	device_status_dimmer();
	//! Return delay for init request deferral (msecs)
	virtual int init_request_delay() { return DIMMER_REQ_DELAY; }
	//~device_status_dimmer();
};

#ifndef DIMMER100_REQ_DELAY
#define DIMMER100_REQ_DELAY 9000
#endif

//! Dimmer 100 status
class device_status_dimmer100 : public device_status {
private:
public:
	enum
	{
		LEV_INDEX = 0,
		OLD_LEV_INDEX,
		SPEED_INDEX,
		FAULT_INDEX,
	} ind;
	//! Return delay for init request deferral (msecs)
	virtual int init_request_delay() { return DIMMER100_REQ_DELAY; }
	device_status_dimmer100();
	//~device_status_dimmer100();
};

//! New timed device status
class device_status_new_timed : public device_status {
private:
public:
	enum
	{
		HH_INDEX = 0,
		MM_INDEX,
		SS_INDEX
	} ind;
	device_status_new_timed();
	//~device_status_temp_new();
};

#ifndef AUTOM_REQ_DELAY
#define AUTOM_REQ_DELAY 6000
#endif

//! Autom device status 
class device_status_autom : public device_status {
private:
public:
	enum
	{
		STAT_INDEX = 0,
	} ind;
	device_status_autom();
	//! Return delay for init request deferral (msecs)
	virtual int init_request_delay() { return AUTOM_REQ_DELAY; }
};

//! Amplifier status
class device_status_amplifier : public device_status {
public:
	enum {
		ON_OFF_INDEX = 0,
		AUDIO_LEVEL_INDEX  = 1,
	} ind;
	device_status_amplifier();
};

//! Radio status
class device_status_radio : public device_status {
public:
	enum {
		FREQ_INDEX = 0,
		STAZ_INDEX,
		RDS0_INDEX,
		RDS1_INDEX,
		RDS2_INDEX,
		RDS3_INDEX,
		RDS4_INDEX,
		RDS5_INDEX,
		RDS6_INDEX,
		RDS7_INDEX
	} ind;
	device_status_radio();
};

//! Sound matrix
class device_status_sound_matr : public device_status {
public:
	enum {
		AMB1_INDEX = 0,
		AMB2_INDEX,
		AMB3_INDEX,
		AMB4_INDEX,
		AMB5_INDEX,
		AMB6_INDEX,
		AMB7_INDEX,
		AMB8_INDEX
	} ind;
	device_status_sound_matr();
};

//! Doorphone status
class device_status_doorphone : public device_status {
public:
	enum {
		PENDING_CALL_INDEX = 0,
	} ind;
	device_status_doorphone();
};

//! Imp. anti status
class device_status_impanti : public device_status {
public:
	enum {
		ON_OFF_INDEX = 0,
	} ind;
	device_status_impanti();
};

//! Zon. anti status
class device_status_zonanti : public device_status {
public:
	enum {
		ON_OFF_INDEX = 0,
	} ind;
	device_status_zonanti();
};

//! Modscen status
class device_status_modscen : public device_status {
public:
	enum {
		STAT_INDEX = 0,
	};
	enum {
		PROGRAMMING_START = 40,
		PROGRAMMING_STOP = 41,
		DELETE_SCEN = 42,
		LOCKED = 43,
		UNLOCKED = 44,
		BUSY = 45,
		FULL = 46,
	} val;
	device_status_modscen();
};

#endif
