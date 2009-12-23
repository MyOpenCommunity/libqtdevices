#ifndef PROBE_DEVICE_H
#define PROBE_DEVICE_H

#include "device.h"


class NonControlledProbeDevice : public device
{
Q_OBJECT
public:
	enum ProbeType
	{
		INTERNAL = 0,
		EXTERNAL = 1
	};

	enum Type
	{
		DIM_TEMPERATURE = 1,
	};

	NonControlledProbeDevice(QString where, ProbeType type);

	void requestStatus();

protected:
	virtual void manageFrame(OpenMsg &msg);

private:
	ProbeType type;
};


class ControlledProbeDevice : public device
{
Q_OBJECT
public:
	enum ProbeType
	{
		NORMAL = 0,
		FANCOIL = 1,
	};

	enum CentralType
	{
		CENTRAL_99ZONES = 0,
		CENTRAL_4ZONES = 1,
	};

	enum Type
	{
		DIM_FANCOIL_STATUS = 1,
		DIM_TEMPERATURE = 2,
		DIM_STATUS = 3,
		DIM_SETPOINT = 4,
		DIM_OFFSET = 5,
		DIM_LOCAL_STATUS = 6,
	};

	enum ProbeStatus
	{
		ST_NONE = 0,
		ST_NORMAL = 0,
		ST_MANUAL = 1,
		ST_AUTO = 2,
		ST_OFF = 3,
		ST_PROTECTION = 4,
	};

	ControlledProbeDevice(QString where, QString central, QString simple_where, CentralType central_type, ProbeType type);

	void setManual(unsigned setpoint);
	void setAutomatic();

	void setFancoilSpeed(int speed);
	void requestFancoilStatus();

	void requestStatus();

protected:
	virtual void manageFrame(OpenMsg &msg);

private slots:
	void timeoutElapsed();

private:
	QString simple_where;
	ProbeType type;
	CentralType central_type;
	bool has_central_info, new_request_allowed;
	QTimer new_request_timer;

	// device state
	ProbeStatus status;
	ProbeStatus local_status;
	int local_offset, set_point;

private:
	static const int TIMEOUT_TIME = 10000;
};

#endif // PROBE_DEVICE_H
