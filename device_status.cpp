
#include "device_status.h"
#include "device_cache.h"

// Status variable implementation
stat_var::stat_var(stat_var::type _t, int _val, int _min, int _max, int _step,
		bool _i)
{
	t = _t;
	val = _val;
	min = _min;
	max = _max;
	step = _step;
	_initialized = _i;
}

stat_var::stat_var(stat_var::type _t)
{
	t = _t;
	val = 0;
	min = 0;
	max = INT_MAX;
	step = 1;
	_initialized = false;
}

void stat_var::get_val(int& out)
{
	out = val;
}

int stat_var::get_val(void) const
{
	return val;
}

void stat_var::set_val(int& in)
{
	if (in > max)
	{
		qDebug("stat_var::set_val(): clamping value to max (%d)", max);
		in = max;
	}
	if (in < min)
	{
		qDebug("stat_var::set_val(): clamping value to min (%d)", min);
		in = min;
	}
	_initialized = true ;
	val = in;
}

void stat_var::get_min(int& out)
{
	out = min;
}

void stat_var::set_min(int& in)
{
	min = in;
}

void stat_var::get_max(int& out)
{
	out = max;
}

void stat_var::set_max(int& in)
{
	max = in;
}

void stat_var::get_step(int& out)
{
	out = step;
}

int stat_var::get_step(void)
{
	return step;
}

void stat_var::set_step(int& in)
{
	step = in;
}

bool stat_var::initialized(void)
{
	return _initialized;
}

void stat_var::invalidate(void)
{
	_initialized = false ;
}

void stat_var::force_initialized(void)
{
	_initialized = true;
}

stat_var::~stat_var()
{
}

// Device status implementation
device_status::device_status(device_status::type _t)
{
	t = _t;
	vars.clear();
	vars.setAutoDelete(true);
	_initialized = false;
	_init_requested = false;
}

device_status::type device_status::get_type(void)
{
	return t;
}

void device_status::set_type(device_status::type& _t)
{
	t = _t;
}

bool device_status::add_var(int index, stat_var *v)
{
	return vars.insert(index, v);
}

int device_status::read(int index, stat_var& out)
{
	stat_var *ptr = vars.at(index);
	if(!ptr)
	{
		qDebug("device_status::read failed!");
		return -1;
	}
	out = *ptr;
	return 0;
}

int device_status::write_val(int index, stat_var& in)
{
	stat_var *ptr = vars.at(index);
	qDebug("device_status::write_val(%d)", index);
	if(!ptr) return -1;
	int v ; in.get_val(v);
	ptr->set_val(v);
	in.set_val(v);
	if(_initialized) 
		return 0;
	// Fixme: do this better
	QPtrListIterator<stat_var> *svi = 
		new QPtrListIterator<stat_var>(vars);
	svi->toFirst();
	stat_var *sv ; 
	while( ( sv = svi->current() ) != 0) {
		if(!sv->initialized()) {
			delete svi;
			return 0;
		}
		++(*svi);
	}
	qDebug("device_status::write_val(): all device status initialized!!");
	_initialized = true ;
	_init_requested = false ;
	delete svi;
	return 0;
}

bool device_status::initialized(void)
{
	return _initialized;
}

bool device_status::init_requested(void)
{
	return _init_requested;
}

void device_status::mark_init_requested(void)
{
	_init_requested = true;
}

void device_status::invalidate(void)
{
	QPtrListIterator<stat_var> *svi = 
		new QPtrListIterator<stat_var>(vars);
	svi->toFirst();
	stat_var *sv ; 
	while( ( sv = svi->current() ) != 0) {
		sv->invalidate();
		++(*svi);
	}
	delete svi;
}

void device_status::force_initialized(void)
{
	QPtrListIterator<stat_var> *svi = 
		new QPtrListIterator<stat_var>(vars);
	svi->toFirst();
	stat_var *sv ; 
	while( ( sv = svi->current() ) != 0) {
		sv->force_initialized();
		++(*svi);
	}
	_initialized = true;
	delete svi;
}

device_status::~device_status()
{
	// Val list is in auto-delete mode, nothing to do
}

// Device status for SUPERVISION MCI
device_status_mci::device_status_mci() : device_status(SUPERVISION_MCI)
{
  add_var((int)device_status_mci::MCI_STATUS_INDEX, new stat_var(stat_var::INFO_MCI, 0, 0, 0xFFFF, 1));
  add_var((int)device_status_mci::MCI_AUTOTEST_FREQ_INDEX, new stat_var(stat_var::WORD, 0, 0, 0xFFFF, 1));
}

bool device_status_mci::GetMciStatusBit(int bit)
{
  stat_var curr_stat(stat_var::INFO_MCI, 0, 0, 0xFFFF, 1); 
  
	read((int)MCI_STATUS_INDEX, curr_stat);

	return (((curr_stat.get_val()>>bit)&1)==0)?false:true;
}

int device_status_mci::GetMciAutotestFreq()
{
  stat_var curr_freq(stat_var::WORD, 0, 0, 0xFFFF, 1); 
  
	read((int)MCI_AUTOTEST_FREQ_INDEX, curr_freq);

	return curr_freq.get_val();
}

bool device_status_mci::SetMciStatus(int s)
{
  qDebug("device_status_mci::SetMciStatus"
	       "(device_status_mci, %d)", s);
  stat_var curr_stat(stat_var::INFO_MCI);
  
	read((int)device_status_mci::MCI_STATUS_INDEX, curr_stat);
  if (!curr_stat.initialized() || (curr_stat.get_val() != s))
  {
	  qDebug("Updating MCI status: %d", s);
	  curr_stat.set_val(s);
	  write_val((int)device_status_mci::MCI_STATUS_INDEX, curr_stat);
	  return true;
  }
	return false;
}

bool device_status_mci::SetMciAutotestFreq(int s)
{
  qDebug("device_status_mci::SetMciAutotestFreq"
	       "(device_status_mci, %d)", s);
  stat_var curr_freq(stat_var::WORD);
  
	read((int)device_status_mci::MCI_AUTOTEST_FREQ_INDEX, curr_freq);
  if (!curr_freq.initialized() || (curr_freq.get_val() != s))
  {
	  qDebug("Updating MCI autotest freq: %d", s);
	  curr_freq.set_val(s);
	  write_val((int)device_status_mci::MCI_AUTOTEST_FREQ_INDEX, curr_freq);
	  return true;
  }
	return false;
}

// Device status for lights
device_status_light::device_status_light() : device_status(LIGHTS)
{
	add_var((int)device_status_light::ON_OFF_INDEX,
			new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
}

// Device status for old dimmers
device_status_dimmer::device_status_dimmer() : device_status(DIMMER)
{
	add_var((int)device_status_dimmer::LEV_INDEX,
			new stat_var(stat_var::LEV, 0, -1, 100, 10));
	add_var((int)device_status_dimmer::OLD_LEV_INDEX,
			new stat_var(stat_var::OLD_LEV, 0, -1, 100, 10));
	add_var((int)device_status_dimmer::FAULT_INDEX,
			new stat_var(stat_var::FAULT, 0, 0, 1, 1));
}

// Device status for dimmer100
device_status_dimmer100::device_status_dimmer100() : device_status(DIMMER100)
{
	add_var((int)device_status_dimmer100::LEV_INDEX,
			new stat_var(stat_var::LEV, 0, -1, 100, 5));
	add_var((int)device_status_dimmer100::OLD_LEV_INDEX,
			new stat_var(stat_var::OLD_LEV, 0, -1, 100, 5));
	add_var((int)device_status_dimmer100::SPEED_INDEX,
			new stat_var(stat_var::SPEED, 1, 1, 255, 1));
	add_var((int)device_status_dimmer100::FAULT_INDEX,
			new stat_var(stat_var::FAULT, 0, 0, 1, 1));
}

// Device status for new timed devices
device_status_new_timed::device_status_new_timed() : device_status(NEWTIMED)
{
	add_var((int)device_status_new_timed::HH_INDEX,
			new stat_var(stat_var::HH, 0, 0, 23, 1));
	add_var((int)device_status_new_timed::MM_INDEX,
			new stat_var(stat_var::MM, 0, 0, 59, 1));
	add_var((int)device_status_new_timed::SS_INDEX,
			new stat_var(stat_var::SS, 0, 0, 59, 1));
}

// Device status for autom. devices
device_status_autom::device_status_autom() :
	device_status(AUTOM)
{
	add_var((int)device_status_autom::STAT_INDEX,
			new stat_var(stat_var::STAT, 0, 0, 2, 1));
}

// Device status for amplifiers
device_status_amplifier::device_status_amplifier() : 
	device_status(AMPLIFIER)
{
	add_var((int)device_status_amplifier::ON_OFF_INDEX,
			new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
	add_var((int)device_status_amplifier::AUDIO_LEVEL_INDEX,
			new stat_var(stat_var::AUDIO_LEVEL, 0, 0, 31, 1));
}

// Device status for radios
device_status_radio::device_status_radio() :
	device_status(RADIO)
{
	add_var((int)device_status_radio::FREQ_INDEX,
			new stat_var(stat_var::FREQ, 0, 0, INT_MAX, 1));
	add_var((int)device_status_radio::STAZ_INDEX,
			new stat_var(stat_var::STAZ, 0, 0, 0xf, 1));
	add_var((int)device_status_radio::RDS0_INDEX,
			new stat_var(stat_var::RDS0, 0, 0, INT_MAX, 1));
	add_var((int)device_status_radio::RDS1_INDEX,
			new stat_var(stat_var::RDS1, 0, 0, INT_MAX, 1));
}

//! Device status for doorphone devices
device_status_doorphone::device_status_doorphone() :
	device_status(DOORPHONE)
{
	add_var((int)device_status_doorphone::PENDING_CALL_INDEX,
			new stat_var(stat_var::PENDING_CALL, 0, 0, 1, 1));
}

// Device status for anti-intrusion system
device_status_impanti::device_status_impanti() : 
	device_status(IMPANTI)
{
	add_var((int)device_status_impanti::ON_OFF_INDEX,
			new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
}

// Device status for anti-intrusion system (single area)
device_status_zonanti::device_status_zonanti() : 
	device_status(ZONANTI)
{
	add_var((int)device_status_zonanti::ON_OFF_INDEX,
			new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
}

device_status_thermal_regulator::device_status_thermal_regulator(device_status::type _type)
	: device_status(_type)
{
	add_var(SP_INDEX, new stat_var(stat_var::SP, 0, 0, INT_MAX, 1));
	add_var(SEASON_INDEX, new stat_var(stat_var::SEASON, 0, 0, 1, 1));
	// programs start at 1 and end at 3
	add_var(PROGRAM_INDEX, new stat_var(stat_var::PROGRAM, 0, 1, 3, 1));
	// scenarios start at 1 and end at 16
	add_var(SCENARIO_INDEX, new stat_var(stat_var::SCENARIO, 0, 1, 16, 1));
	add_var(STATUS_INDEX, new stat_var(stat_var::THERMR, -1, OFF, WEEKEND, 1));
}

device_status_thermal_regulator_4z::device_status_thermal_regulator_4z()
	: device_status_thermal_regulator(THERMAL_REGULATOR_4Z)
{
}
device_status_thermal_regulator_99z::device_status_thermal_regulator_99z()
	: device_status_thermal_regulator(THERMAL_REGULATOR_99Z)
{
}

device_status_temperature_probe::device_status_temperature_probe() :
	device_status(TEMPERATURE_PROBE)
{
	add_var((int)TEMPERATURE_INDEX, new stat_var(stat_var::TEMPERATURE, 0, -999, 999, 1));
}

device_status_temperature_probe_extra::device_status_temperature_probe_extra(thermo_type_t type) :
	device_status(TEMPERATURE_PROBE_EXTRA)
{
	/*
	 * Initial value -1: not a possible one, so when the probe is
	 * read the first time, we are sure the previous value is different
	 * and the relevant display item is refreshed.
	 */
	add_var((int)STAT_INDEX, new stat_var(stat_var::STAT, -1, 0, 5, 1));
	add_var((int)LOCAL_INDEX, new stat_var(stat_var::LOCAL, -1, 0, 13, 1));
	add_var((int)SP_INDEX, new stat_var(stat_var::SP, 0, 0, INT_MAX, 1));
	add_var((int)CRONO, new stat_var(stat_var::CRONO, 0, 0, 1, 1));
	add_var((int)INFO_SONDA, new stat_var(stat_var::INFO_SONDA, 0, 0, INT_MAX, 1));
	add_var((int)INFO_CENTRALE, new stat_var(stat_var::INFO_CENTRALE, 1, 0, 1, 1));
}

// Device status for fancoil
device_status_fancoil::device_status_fancoil() :
	device_status(FANCOIL)
{
	/*
	 * Initial value -1: not a possible one, so when the probe is
	 * read the first time, we are sure the previous value is different
	 * and the relevant display item is refreshed.
	 */
	add_var((int)SPEED_INDEX, new stat_var(stat_var::FANCOIL_SPEED, -1, 0, 3, 1));
}

// Device status for modscen
device_status_modscen::device_status_modscen() :
	device_status(MODSCEN)
{
	add_var((int)device_status_modscen::STAT_INDEX,
			new stat_var(stat_var::STAT, 0, 
				device_status_modscen::PROGRAMMING_START, 
				device_status_modscen::FULL, 1));
}

// Device status for sound matrix
device_status_sound_matr::device_status_sound_matr() :
	device_status(SOUNDMATR)
{
	add_var((int)device_status_sound_matr::AMB1_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB2_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB3_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB4_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB5_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB6_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB7_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
	add_var((int)device_status_sound_matr::AMB8_INDEX,
			new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
}
