#include "loads_device.h"

#include "openmsg.h"

enum
{
	_DIM_STATUS = 71,
	_DIM_TOTALS = 72,
};


LoadsDevice::LoadsDevice(const QString &where) :
	device("18", where)
{
}

void LoadsDevice::init() const
{
	requestCurrent();
	requestLevel();
	requestStatus();
	requestTotal(0);
	requestTotal(1);
}

void LoadsDevice::enable() const
{
	sendCommand("71");
}

void LoadsDevice::forceOn() const
{
	// TODO to be confirmed
	sendCommand("74");
}

void LoadsDevice::forceOff(int time) const
{
	// TODO to be confirmed
	sendCommand(QString("73#%1").arg(time / 10));
}

void LoadsDevice::requestCurrent() const
{
	sendRequest("113");
}

void LoadsDevice::requestStatus() const
{
	sendRequest("71");
}

void LoadsDevice::requestTotal(int period) const
{
	sendRequest(QString("72#%1").arg(period + 1));
}

void LoadsDevice::requestLevel() const
{
	sendRequest("73");
}

void LoadsDevice::resetTotal(int period) const
{
	sendCommand(QString("75#%1").arg(period + 1));
}

void LoadsDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));
	if (who.toInt() != msg.who() || where.toStdString() != msg.whereFull())
		return;

	int what = msg.what();
	StatusList status_list;

	if (what == _DIM_STATUS && msg.whatArgCnt() == 5)
	{
		status_list[DIM_ENABLED] = msg.whatArgN(0) == 0;
		status_list[DIM_FORCED] = msg.whatArgN(1) == 1;
		status_list[DIM_ABOVE_THRESHOLD] = msg.whatArgN(2) == 0;
		status_list[DIM_PROTECTION] = msg.whatArgN(3) == 1;
	}
	else if (what == _DIM_TOTALS && msg.whatArgCnt() == 6 && msg.whatSubArgCnt() == 1)
	{
		status_list[DIM_PERIOD] = msg.whatSubArgN(0) - 1;
		status_list[DIM_TOTAL] = msg.whatArgN(0);

		QDate d(msg.whatArgN(3), msg.whatArgN(2), msg.whatArgN(1));
		QTime t(msg.whatArgN(4), msg.whatArgN(5), 0);

		status_list[DIM_RESET_DATE] = QDateTime(d, t);
	}
	else if (what == DIM_CURRENT && msg.whatArgCnt() == 1)
	{
		status_list[DIM_CURRENT] = msg.whatArgN(0);
	}
	else if (what == DIM_LOAD && msg.whatArgCnt() == 1)
	{
		status_list[DIM_LOAD] = msg.whatArgN(0);
	}

	if (status_list.count() != 0)
		emit status_changed(status_list);
}
