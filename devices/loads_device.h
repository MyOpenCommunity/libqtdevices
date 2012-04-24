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


#ifndef LOADS_DEVICE_H
#define LOADS_DEVICE_H

#include "device.h"

class AutomaticUpdates;


/*!
	\ingroup LoadManagement
	\ingroup LoadDiagnostic
	\brief Device that controls the loads management/diagnostic system.

	This device can be used to monitor the actual loads as well as log
	consuptions on long periods.

	The actual absorption can be retrived using the requestCurrent() method.
	Automatic updates of this value can be enabled or disabled using the
	requestCurrentUpdateStart() and requestCurrentUpdateStop() methods.

	The total absorption can be retrived using the requestTotal() method which
	takes the period as a paremeter. The device can log two distinct periods.
	To reset the counter, you can use the resetTotal() method.

	\section LoadsDevice-dimensions Dimensions
	\startdim
	\dim{DIM_ENABLED,bool,,True if enabled\, false otherwise}
	\dim{DIM_FORCED,bool,,True if forced\, false otherwise}
	\dim{DIM_ABOVE_THRESHOLD,bool,,True if above threshold\, false otherwise
	(not used for now)}
	\dim{DIM_PROTECTION,bool,,True if load protection triggered\, false otherwise
	(not used for now)}
	\dim{DIM_PERIOD,int,0\,1,the device keeps track of total consuption for 2
	indipendent time periods\, this measure indicates which one. To be used in
	conjunction with DIM_TOTAL and DIM_RESET_DATE}
	\dim{DIM_TOTAL,int,,Total absorpion for this time period. Period refers to
	the one specified by DIM_PERIOD}
	\dim{DIM_RESET_DATE,QDateTime,,Last reset of the counter for this time period.
	Period refers to the one specified by DIM_PERIOD}
	\dim{DIM_CURRENT,int,,Current absorption value\, expressed in Watt}
	\dim{DIM_LOAD,LoadsDevice::LoadLevel,,Load level\, see LoadsDevice::LoadLevel}
	\enddim

	\sa \ref device-dimensions
*/
class LoadsDevice : public device
{
friend class TestLoadsDevice;
Q_OBJECT
public:
	/*!
		\refdim{LoadsDevice}
	*/
	enum Type
	{
		DIM_ENABLED,
		DIM_FORCED,
		DIM_ABOVE_THRESHOLD,
		DIM_PROTECTION,
		DIM_PERIOD,
		DIM_TOTAL,
		DIM_RESET_DATE,
		DIM_CURRENT = 113,
		DIM_LOAD = 73,
	};

	enum LoadLevel
	{
		LOAD_OK = 1,         /*!< Load within bounds */
		LOAD_WARNING = 2,    /*!< Load near trigger point */
		LOAD_CRITICAL = 3,   /*!< Load above trigger point */
	};

	/*!
		\brief Constructor.

		Constructs a new LoadsDevice with the given \a where.
	*/
	LoadsDevice(const QString &where, int openserver_id = 0);

	~LoadsDevice();
	virtual void init();

	/*!
		\brief Forces the device to off for the given \a time (expressed in minutes)
	*/
	void forceOff(int time) const;

	/*!
		\brief Requests current absorption
	*/
	void requestCurrent() const;

	/*!
		\brief Starts the automatic updates.
	*/
	void requestCurrentUpdateStart();

	/*!
		\brief Stops the automatic updates.
	*/
	void requestCurrentUpdateStop();

	/*!
		\brief Requests the status for the actuator.
	*/
	void requestStatus() const;

	/*!
		\brief Requests the total absorption for \a period.

		\note Valid values for \a period are 0 or 1.
	*/
	void requestTotal(int period) const;

	/*!
		\brief Resets the total absorption.

		\note Valid values for \a period are 0 or 1.
	*/
	void resetTotal(int period) const;

	/*!
		\brief Requests the differential level.
	*/
	void requestLevel() const;

public slots:
	/*!
		\brief Enables an actuator disabled by the central unit.
	*/
	void enable() const;

	/*!
		\brief Forces the device to on.
	*/
	void forceOn() const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	// handle automatic updates of current measure
	AutomaticUpdates *current_updates;
};

#endif // LOADS_DEVICE_H
