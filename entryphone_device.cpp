#include "entryphone_device.h"

#include <openmsg.h>

EntryphoneDevice::EntryphoneDevice(const QString &where) :
	device(QString("8"), where)
{
}

void EntryphoneDevice::answerCall() const
{
}

void EntryphoneDevice::callEnd() const
{
}

void EntryphoneDevice::autoSwitching() const
{
}

void EntryphoneDevice::cycleCamera() const
{
}

void EntryphoneDevice::manageFrame(OpenMsg &msg)
{
}
