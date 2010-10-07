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


#include "device_status.h"
#include <limits.h>

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

int stat_var::get_val() const
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

int stat_var::get_step()
{
	return step;
}

void stat_var::set_step(int& in)
{
	step = in;
}

bool stat_var::initialized()
{
	return _initialized;
}

void stat_var::invalidate()
{
	_initialized = false ;
}

void stat_var::force_initialized()
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
	while (!vars.isEmpty())
		delete vars.takeFirst();

	_initialized = false;
	_init_requested = false;
}

device_status::type device_status::get_type()
{
	return t;
}

void device_status::set_type(device_status::type& _t)
{
	t = _t;
}

void device_status::add_var(int index, stat_var *v)
{
	vars.insert(index, v);
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
	if (!ptr)
		return -1;

	int v;
	in.get_val(v);
	ptr->set_val(v);
	in.set_val(v);
	if (_initialized) 
		return 0;

  	for (int i = 0; i < vars.size(); ++i)
		if (!vars.at(i)->initialized())
			return 0;

	qDebug("device_status::write_val(): all device status initialized!!");
	_initialized = true ;
	_init_requested = false ;
	return 0;
}

bool device_status::initialized()
{
	return _initialized;
}

bool device_status::init_requested()
{
	return _init_requested;
}

void device_status::mark_init_requested()
{
	_init_requested = true;
}

void device_status::invalidate()
{
  	for (int i = 0; i < vars.size(); ++i)
		vars.at(i)->invalidate();
}

void device_status::force_initialized()
{
  	for (int i = 0; i < vars.size(); ++i)
		vars.at(i)->force_initialized();
}

device_status::~device_status()
{
	while (!vars.isEmpty())
		delete vars.takeFirst();
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

