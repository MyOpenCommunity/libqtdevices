#ifndef LOADS_DEVICE_H
#define LOADS_DEVICE_H

#include "device.h"


class LoadsDevice : public device
{
friend class TestLoadsDevice;
Q_OBJECT
public:
	enum
	{
		// true if enabled, false otherwise
		DIM_ENABLED,

		// true if forced, false otherwise
		DIM_FORCED,

		// true if above thershold, false otherwise (not used for now)
		DIM_ABOVE_THRESHOLD,

		// true if load protection triggered, false otherwise (not used for now)
		DIM_PROTECTION,

		// used together:
		// can be 0 or 1, the device keeps track of total consumption
		// for 2 indipendent time periods
		DIM_PERIOD,
		// total absorption for this time period
		DIM_TOTAL,
		// last reset of the counter (QDateTime)
		DIM_RESET_DATE,

		// current absorption value, in Watt
		DIM_CURRENT = 113,

		// load level, see enum LoadLevel
		DIM_LOAD = 73,
	};

	enum LoadLevel
	{
		// load within bounds
		LOAD_OK = 1,

		// load near trigger point
		LOAD_WARNING = 2,

		// load above trigger point
		LOAD_CRITICAL = 3,
	};

	LoadsDevice(const QString &where);

	// enable actuator
	void enable() const;

	// force the device to on
	void forceOn() const;

	// force the device to off for the given time, expressed in minutes
	void forceOff(int time) const;

	// request current absorption
	void requestCurrent() const;

	// request the status for the actuator
	void requestStatus() const;

	// request total absorption
	void requestTotal(int period) const;

	// reset total absorption
	void resetTotal(int period) const;

	// request the differential level
	void requestLevel() const;

public slots:
	void frame_rx_handler(char *frame);
};

#endif // LOADS_DEVICE_H
