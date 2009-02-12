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

void EnergyDevice::requestDailyAverageGraph(QDate date) const
{
	sendFrame(createMsgOpen(who, QString("53#%1").arg(date.month()), where));
	buffer_frame.clear();
}

void EnergyDevice::requestDayGraph(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2#%3").arg(DIM_TX_DAY_GRAPH)
		.arg(date.month()).arg(date.day()), where));
	buffer_frame.clear();
}

void EnergyDevice::requestCumulativeMonth(QDate date) const
{
	QDate curr = QDate::currentDate();

	if (date.year() == curr.year() && date.month() == curr.month())
		sendRequest(DIM_CUMULATIVE_MONTH);
	else
	{
		int year = date.toString("yy").toInt();
		sendRequest(QString("%1#%2#%3").arg(_DIM_CUMULATIVE_MONTH).arg(year).arg(date.month()));
	}
}

void EnergyDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || where.toInt() != msg.where())
		return;

	int what = msg.what();

	StatusList status_list;
	QVariant v;

	if (what == DIM_CUMULATIVE_DAY || what == DIM_CURRENT || what == DIM_CUMULATIVE_MONTH ||
		what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_YEAR || what == DIM_DAILY_AVERAGE ||
		what == DIM_RX_DAY_GRAPH)
	{
		qDebug("EnergyDevice::frame_rx_handler -> frame read:%s", frame);

		if (what == DIM_DAILY_AVERAGE)
		{
			// We assume that the frames came in correct (sequential) order
			int num_frame = msg.whatArgN(0);
			if (num_frame > 0 && num_frame < 18)
				buffer_frame.append(frame);

			OpenMsg frame_parser;
			QList<int> values_list;
			for (int i = 0; i < buffer_frame.size(); ++i)
			{
				frame_parser.CreateMsgOpen(buffer_frame[i].toAscii().data(), buffer_frame[i].length());
				values_list.append(frame_parser.whatArgN(1));
				if (frame_parser.whatArgN(0) != 17)
				{
					values_list.append(frame_parser.whatArgN(2));
					if (frame_parser.whatArgN(0) != 1)
						values_list.append(frame_parser.whatArgN(3));
				}
			}

			GraphData data;
			for (int i = 0; i + 1 < values_list.size(); i+=2)
				data[i / 2 + 1] = values_list[i] * 256 + values_list[i + 1];

			v.setValue(data);
			status_list[what] = v;
		}
		else if (what == DIM_RX_DAY_GRAPH)
		{
			int num_frame = msg.whatArgN(0);
			if (num_frame > 0 && num_frame < 10)
				buffer_frame.append(frame);
			status_list[what] = parseDayGraph(buffer_frame, msg);
		}
		else
		{
			assert(msg.whatArgCnt() == 1);
			int val = msg.whatArgN(0);
			v.setValue(val);

			if (what == _DIM_CUMULATIVE_MONTH)
				status_list[DIM_CUMULATIVE_MONTH] = v;
			else
				status_list[what] = v;
		}
		emit status_changed(status_list);
	}

}

QVariant EnergyDevice::parseDayGraph(const QList<QString> &buffer_frame, OpenMsg &msg)
{
	QList<int> values;
	// assume that frames arrive in order
	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		assert(msg.whatArgCnt() > 1);
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		if (frame_parser.whatArgN(0) == 1)
			values.append(frame_parser.whatArgN(2));
		else if (frame_parser.whatArgN(0) == 9)
		{

			values.append(frame_parser.whatArgN(1));
			values.append(frame_parser.whatArgN(2));
		}
		else
			for (int j = 1; j < 4; ++j)
				values.append(frame_parser.whatArgN(j));
	}

	GraphData data;
	for (int i = 0; i < values.size(); ++i)
		data[i + 1] = values[i] == MAX_VALUE ? 0 : values[i];
	QVariant v;
	v.setValue(data);
	return v;
}
