#ifndef __DEVICE_STATUS_H__
#define __DEVICE_STATUS_H__

#include <qptrlist.h>

/*
 * Not ideal here, thermo specific stuff, but
 * that would imply major code shuffling or strange
 * forward declarations.
 */
enum thermo_type_t
{
	THERMO_Z99,  // 99 zones thermal regulator
	THERMO_Z4,   // 4 zones thermal regulator
};

//! State variable
class stat_var
{
public:
	enum type
	{
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
		LOCAL,   // Temperature probe personal setpoint delta
		SP,      // Temperature probe setpoint
		ACTIVE_SOURCE,
		FAULT,
		CRONO,   // Secret, who knows?
		INFO_SONDA,
		INFO_CENTRALE,
		FANCOIL_SPEED,
		PROGRAM, // weekly program currently set in the thermal regulator
		SCENARIO, // scenario currentrly set in thermal regulator (99 zones only)
		SEASON,   // summer / winter
	};
private:
	type t;
	//QString descr;
	//! Value
	int val;
	//! Minimum
	int min;
	//! Maximum
	int max;
	//! Step
	int step;
	//! True when initialized
	bool _initialized;
public:
	//! Constructor
	stat_var(stat_var::type _t, int val, int min, int max, 
			int step, bool initialized = false);
	//! Reduced constructor
	stat_var(stat_var::type);
	//! Get value
	void get_val(int&);
	//! Get value again
	int get_val(void) const;
	//! Set value
	void set_val(int&);
	//! Get min value
	void get_min(int&);
	//! Set min value
	void set_min(int&);
	//! Get max value
	void get_max(int&);
	//! Set max value
	void set_max(int&);
	//! Get step
	void get_step(int&);
	//! Get step, alternate form
	int get_step(void);
	//! Set step
	void set_step(int&);
	//! Get initialized status
	bool initialized(void);
	//! Invalidate variable contents
	void invalidate(void);
	//! Force initialized state
	void force_initialized(void);
	//! Destructor
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
		TEMPERATURE_PROBE,
		AMPLIFIER,
		RADIO,
		DOORPHONE,
		IMPANTI,
		ZONANTI,
		TEMPERATURE_PROBE_EXTRA,
		FANCOIL,
		MODSCEN,
		SOUNDMATR,
		THERMAL_REGULATOR_4Z,
		THERMAL_REGULATOR_99Z,
	} type;
private:
	//! Type
	type t;
	//! Variables
	QPtrList<stat_var> vars;
	//! True when initialized
	bool _initialized;
	//! True when init requested
	bool _init_requested;
public:
	//! Constructor
	device_status(device_status::type);
	//! Read type
	device_status::type get_type(void);
	//! Set type
	void set_type(device_status::type& t);
	//! Add variable
	bool add_var(int index, stat_var *);
	//! Read variable
	int read(int index, stat_var& out);
	//! Write variable's value
	int write_val(int index, stat_var& val);
	//! Returns true when status initialized
	bool initialized(void);
	//! Returns true when status initialize has already been requested
	bool init_requested(void);
	//! Sets init_requested flag
	void mark_init_requested(void);
	//! Invalidate device status
	void invalidate(void);
	//! Force initialized status
	void force_initialized(void);
	//! Return delay for init request deferral (msecs)
	virtual int init_request_delay() { return 0; }
	//! Destructor
	virtual ~device_status();
};

#ifndef LIGHT_REQ_DELAY
#define LIGHT_REQ_DELAY 1500
#endif


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
#define DIMMER100_REQ_DELAY 4000
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

/**
 * Device status for thermal regulator device
 */
class device_status_thermal_regulator_4z : public device_status
{
public:
	enum
	{
		SP_INDEX = 0,
		SEASON_INDEX,
		PROGRAM_INDEX,
	} ind;

	device_status_thermal_regulator_4z();
};

/**
 * Device status for thermal regulator device
 */
class device_status_thermal_regulator_99z : public device_status
{
public:
	enum
	{
		SP_INDEX = 0,
		SEASON_INDEX,
		PROGRAM_INDEX,
		SCENARIO_INDEX,
	} ind;

	device_status_thermal_regulator_99z();
};

/**
 * Device status for extra thermal information present only in a controlled probe.
 */
class device_status_temperature_probe_extra : public device_status {
public:
	enum {
		STAT_INDEX = 0,
		LOCAL_INDEX,
		SP_INDEX,
		CRONO,
		INFO_SONDA,
		INFO_CENTRALE,
	} ind;
	enum {
		S_MAN = 0,
		S_AUTO,
		S_ANTIGELO,
		S_TERM,
		S_GEN,
		S_OFF,
		S_NONE,  // 4 zones: no status
	} val;

	device_status_temperature_probe_extra(thermo_type_t);
};

/**
 * Device status for temperature measured by temperature probes, thermal regulators, etc.
 */
class device_status_temperature_probe : public device_status {
public:
	enum {
		TEMPERATURE_INDEX = 0,
	} ind;
	device_status_temperature_probe();
};

/**
 * Fancoil element present in some controlled temperature probes.
 */
class device_status_fancoil : public device_status {
public:
	enum {
		SPEED_INDEX = 0,
	} ind;
	device_status_fancoil();
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
