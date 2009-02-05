#include "energy_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>

#include <assert.h>


EnergyDevice::EnergyDevice(QString where) : device(QString("18"), where)
{

}

void EnergyDevice::sendRequest(int what) const
{
	sendInit(createRequestOpen(who, QString::number(what), where));
}

void EnergyDevice::requestCumulativeDay() const
{
	sendRequest(54);
}

void EnergyDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || where.toInt() != msg.where())
		return;

	int what = msg.what();
	int what_args = msg.whatArgCnt();

	QHash<int, QVariant> status_list;
	QVariant v;

	if (what == 54)
	{
		assert(what_args == 1);
		int val = msg.whatArgN(0);
		v.setValue(val);

		status_list[what] = v;
		emit status_changed(status_list);
	}

}
