#ifndef LOADS_DEVICE_H
#define LOADS_DEVICE_H

#include "device.h"
#include "energy_device.h" // for AutomaticUpdates


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

	virtual void init(bool forza);

	// force the device to off for the given time, expressed in minutes
	void forceOff(int time) const;

	// request current absorption
	void requestCurrent() const;

	// start/stop automatic updates
	void requestCurrentUpdateStart();
	void requestCurrentUpdateStop();

	// request the status for the actuator
	void requestStatus() const;

	// request total absorption
	void requestTotal(int period) const;

	// reset total absorption
	// can be 0 for first period, 1 for second period
	void resetTotal(int period) const;

	// request the differential level
	void requestLevel() const;

public slots:
	// enable an actuator disabled by the central unit
	void enable() const;

	// force the device to on
	void forceOn() const;

	void frame_rx_handler(char *frame);

private:
	// handle automatic updates of current measure
	AutomaticUpdates current_updates;
};

#endif // LOADS_DEVICE_H
