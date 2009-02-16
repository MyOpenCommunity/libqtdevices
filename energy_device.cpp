#include "energy_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>
#include <QDate>

#include <assert.h>

enum RequestDimension
{
	_DIM_CUMULATIVE_MONTH = 52, // An implementation detail, ignore this

	REQ_DAILY_AVERAGE_GRAPH      = 53,   // graph data for daily average
	REQ_DAY_GRAPH                = 52,   // request graph data for a specific day
	REQ_CUMULATIVE_MONTH_GRAPH   = 56,   // request graph data for cumulative month
};

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
	sendFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_DAILY_AVERAGE_GRAPH)
		.arg(date.month()), where));
}

void EnergyDevice::requestCumulativeDayGraph(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2#%3").arg(REQ_DAY_GRAPH)
		.arg(date.month()).arg(date.day()), where));
}

void EnergyDevice::requestCumulativeMonthGraph(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_CUMULATIVE_MONTH_GRAPH)
		.arg(date.month()), where));
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
		what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_YEAR || what == ANS_DAILY_AVERAGE_GRAPH ||
		what == ANS_DAY_GRAPH || what == ANS_CUMULATIVE_MONTH_GRAPH)
	{
		assert(msg.whatArgCnt() > 0);
		qDebug("EnergyDevice::frame_rx_handler -> frame read:%s", frame);
		int num_frame = msg.whatArgN(0);
		// clear the buffer if the first frame of a new graph arrives
		if (num_frame == 1 && msg.whatArgCnt() > 1)
			buffer_frame.clear();

		if (what == ANS_DAILY_AVERAGE_GRAPH)
		{
			// We assume that the frames came in correct (sequential) order
			if (num_frame > 0 && num_frame < 18)
				buffer_frame.append(frame);

			status_list[what] = parseDailyAverageGraph(buffer_frame);
		}
		else if (what == ANS_DAY_GRAPH)
		{
			if (num_frame > 0 && num_frame < 10)
				buffer_frame.append(frame);
			status_list[what] = parseDayGraph(buffer_frame, msg);
		}
		else if (what == ANS_CUMULATIVE_MONTH_GRAPH)
		{
			if (num_frame > 0 && num_frame < 22)
				buffer_frame.append(frame);
			status_list[what] = parseCumulativeMonthGraph(buffer_frame);
		}
		else
		{
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

QVariant EnergyDevice::parseDailyAverageGraph(const QList<QString> &buffer_frame)
{
	QList<int> values_list;
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		values_list.append(frame_parser.whatArgN(1));
		if (frame_parser.whatArgN(0) != 17)
		{
			values_list.append(frame_parser.whatArgN(2));
			if (frame_parser.whatArgN(0) != 1)
				values_list.append(frame_parser.whatArgN(3));
		}
	}

	for (int i = 0; i + 1 < values_list.size(); i+=2)
		data.graph[i / 2 + 1] = values_list[i] * 256 + values_list[i + 1];

	QVariant v;
	v.setValue(data);
	return v;
}

QVariant EnergyDevice::parseDayGraph(const QList<QString> &buffer_frame, OpenMsg &msg)
{
	QList<int> values;
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);

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

	for (int i = 0; i < values.size(); ++i)
		data.graph[i + 1] = values[i] == MAX_VALUE ? 0 : values[i];
	QVariant v;
	v.setValue(data);
	return v;
}

QVariant EnergyDevice::parseCumulativeMonthGraph(const QList<QString> &buffer_frame)
{
	GraphData data;
	QList<int> values;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	// TODO: data.type;

	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		assert(frame_parser.whatArgCnt() > 1);
		values.append(frame_parser.whatArgN(1));
		values.append(frame_parser.whatArgN(2));
		if (frame_parser.whatArgN(0) != 1)
			values.append(frame_parser.whatArgN(3));
	}

	for (int i = 0; i + 1 < values.size(); i += 2)
	{
		int high = values[i] == 255 ? 0 : values[i];
		int low = values[i+1] == 255 ? 0 : values[i+1];
		data.graph[i / 2 + 1] = high * 256 + low;
	}

	QVariant v;
	v.setValue(data);
	return v;
}

QDate EnergyDevice::getDateFromFrame(OpenMsg &msg)
{
	assert(msg.whatSubArgCnt() > 0);
	QDate current = QDate::currentDate();
	int month = msg.whatSubArgN(0);
	int day = msg.whatSubArgCnt() > 1 ? msg.whatSubArgN(1) : 1;
	int year = month <= current.month() ? current.year() : current.year() - 1;
	return QDate(year, month, day);
}
