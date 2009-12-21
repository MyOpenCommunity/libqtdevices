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

#endif // PROBE_DEVICE_H

