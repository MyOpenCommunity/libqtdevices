#include "probe_device.h"
#include "openmsg.h"

#include <QtDebug>

enum what_t
{
	TEMPERATURE = 0,
	EXTERNAL_TEMPERATURE = 15
};

NonControlledProbeDevice::NonControlledProbeDevice(QString where, ProbeType t)
	: device(QString("4"), where),
	type(t)
{
}

void NonControlledProbeDevice::requestStatus()
{
	sendRequest(type == EXTERNAL ? "15#1" : "0");
}

void NonControlledProbeDevice::manageFrame(OpenMsg &msg)
{
	if (where.toInt() != msg.where())
		return;

	if (type == EXTERNAL && msg.what() == EXTERNAL_TEMPERATURE)
	{
		StatusList sl;

		sl[DIM_TEMPERATURE] = msg.whatArgN(1);
		emit status_changed(sl);
	}
	else if (type == INTERNAL && msg.what() == TEMPERATURE)
	{
		StatusList sl;

		sl[DIM_TEMPERATURE] = msg.whatArgN(0);
		emit status_changed(sl);
	}
	else
		qDebug() << "Unhandled frame" << msg.frame_open;
}
