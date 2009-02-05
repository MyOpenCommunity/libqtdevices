#include "energy_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>
#include <QDate>

#include <assert.h>


EnergyDevice::EnergyDevice(QString where) : device(QString("18"), where)
{
}

void EnergyDevice::sendRequest(int what) const
{
	sendInit(createRequestOpen(who, QString::number(what), where));
}

void EnergyDevice::sendRequest(QString what) const
{
	sendInit(createRequestOpen(who, what, where));
}

void EnergyDevice::requestCumulativeDay() const
{
	sendRequest(DIM_CUMULATIVE_DAY);
}

void EnergyDevice::requestCurrent() const
{
	sendRequest(DIM_CURRENT);
}

void EnergyDevice::requestCumulativeYear() const
{
	sendRequest(DIM_CUMULATIVE_YEAR);
}

void EnergyDevice::requestCumulativeMonth(QDate date) const
{
	QDate curr = QDate::currentDate();
	if (date.year() == curr.year() && date.month() == curr.month())
		sendRequest(DIM_CUMULATIVE_MONTH);
	else
		sendRequest(QString("%1#%2#%3").arg(_DIM_CUMULATIVE_MONTH).arg(date.year()).arg(date.month()));
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

	if (what == DIM_CUMULATIVE_DAY || what == DIM_CURRENT || what == DIM_CUMULATIVE_MONTH ||
		what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_YEAR)
	{
		qDebug("EnergyDevice::frame_rx_handler -> frame read:%s", frame);
		assert(what_args == 1);
		int val = msg.whatArgN(0);
		v.setValue(val);

		if (what == _DIM_CUMULATIVE_MONTH)
			status_list[DIM_CUMULATIVE_MONTH] = v;
		else
			status_list[what] = v;

		emit status_changed(status_list);
	}

}
