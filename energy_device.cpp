#include "energy_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>
#include <QtGlobal>
#include <QList>

const int MAX_VALUE = 255;


namespace
{
	inline int getValue(int high, int low)
	{
		return high == MAX_VALUE && low == MAX_VALUE ? 0 : high * 256 + low;
	}
}

enum RequestDimension
{
	_DIM_CUMULATIVE_MONTH = 52, // An implementation detail, ignore this

	REQ_DAILY_AVERAGE_GRAPH      = 53,   // graph data for daily average
	REQ_DAY_GRAPH                = 52,   // request graph data for a specific day
	REQ_CUMULATIVE_MONTH_GRAPH   = 56,   // request graph data for cumulative month
};


enum RequestCurrent
{
	REQ_CURRENT_MODE_1 = 113,
	REQ_CURRENT_MODE_2 = 1134,
	REQ_CURRENT_MODE_3 = 1130,
	REQ_CURRENT_MODE_4 = 1132,
	REQ_CURRENT_MODE_5 = 1132,
};


EnergyDevice::EnergyDevice(QString where, int _mode) : device(QString("18"), where)
{
	mode = _mode;

	for (int i = 1; i <= 12; ++i)
		buffer_year_data[i] = 0;
}

void EnergyDevice::sendRequest(int what, bool use_compressed_init) const
{
	sendRequest(QString::number(what), use_compressed_init);
}

void EnergyDevice::sendRequest(QString what, bool use_compressed_init) const
{
	QString req = createRequestOpen(who, what, where);
	if (use_compressed_init)
		sendCompressedInit(req);
	else
		sendInit(req);
}

void EnergyDevice::requestCumulativeDay(QDate date) const
{
	if (date == QDate::currentDate())
		sendRequest(DIM_CUMULATIVE_DAY);
	else
		requestCumulativeDayGraph(date);
}

void EnergyDevice::requestCurrent() const
{
	int what;
	switch (mode)
	{
	case 1:
		what = REQ_CURRENT_MODE_1;
		break;
	case 2:
		what = REQ_CURRENT_MODE_2;
		break;
	case 3:
		what = REQ_CURRENT_MODE_3;
		break;
	case 4:
		what = REQ_CURRENT_MODE_4;
		break;
	case 5:
		what = REQ_CURRENT_MODE_5;
		break;
	default:
		qFatal("Unknown mode on the energy management!");
	}
	sendRequest(what);
}

void EnergyDevice::requestCumulativeYear() const
{
	sendRequest(DIM_CUMULATIVE_YEAR);
}

void EnergyDevice::requestDailyAverageGraph(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_DAILY_AVERAGE_GRAPH)
		.arg(date.month()), where));
}

void EnergyDevice::requestMontlyAverage(QDate date) const
{
	requestCumulativeMonth(date);
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
	// The public method is not compressed.
	requestCumulativeMonth(date, false);
}

void EnergyDevice::requestCumulativeMonth(QDate date, bool use_compressed_init) const
{
	QDate curr = QDate::currentDate();

	if (date.year() == curr.year() && date.month() == curr.month())
		sendRequest(DIM_CUMULATIVE_MONTH, use_compressed_init);
	else
	{
		int year = date.toString("yy").toInt();
		sendRequest(QString("%1#%2#%3").arg(_DIM_CUMULATIVE_MONTH).arg(year).arg(date.month()), use_compressed_init);
	}
}

void EnergyDevice::requestCumulativeYearGraph() const
{
	QDate curr = QDate::currentDate();
	for (int i = 0; i < 12; ++i)
		requestCumulativeMonth(curr.addMonths(i * -1), true); // we compress the request of the graph data
}

void EnergyDevice::manageFrame(OpenMsg &msg)
{
	if (where.toInt() != msg.where())
		return;

	int what = msg.what();

	StatusList status_list;
	QVariant v;

	if (what == DIM_CUMULATIVE_DAY || what == REQ_CURRENT_MODE_1 || what == REQ_CURRENT_MODE_2 ||
		what == REQ_CURRENT_MODE_3 || what == REQ_CURRENT_MODE_4 || what == REQ_CURRENT_MODE_5 ||
		what == DIM_CUMULATIVE_MONTH || what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_YEAR ||
		what == DIM_DAILY_AVERAGE_GRAPH || what == DIM_DAY_GRAPH || what == DIM_CUMULATIVE_MONTH_GRAPH)
	{

		// In some cases (when more than a ts is present in the system)
		// a request frame can arrive from the monitor socket. We have to manage this
		// situation.
		if (!msg.whatArgCnt() || msg.IsStateFrame() || msg.IsNormalFrame())
			return;

		qDebug("EnergyDevice::manageFrame -> frame read:%s", msg.frame_open);
		int num_frame = msg.whatArgN(0);
		// clear the buffer if the first frame of a new graph arrives
		if (num_frame == 1 && msg.whatArgCnt() > 1)
			buffer_frame.clear();

		if (what == DIM_DAILY_AVERAGE_GRAPH)
		{
			// We assume that the frames came in correct (sequential) order
			if (num_frame > 0 && num_frame < 18)
				buffer_frame.append(msg.frame_open);
			parseDailyAverageGraph(buffer_frame, v);
		}
		else if (what == DIM_DAY_GRAPH)
		{
			if (num_frame > 0 && num_frame < 10)
				buffer_frame.append(msg.frame_open);
			parseCumulativeDayGraph(buffer_frame, v);
		}
		else if (what == DIM_CUMULATIVE_MONTH_GRAPH)
		{
			if (num_frame > 0 && num_frame < 22)
				buffer_frame.append(msg.frame_open);
			parseCumulativeMonthGraph(buffer_frame, v);
		}
		else
		{
			int val = msg.whatArg(0) == "4294967295" ? 0 : msg.whatArgN(0);
			v.setValue(EnergyValue(getDateFromFrame(msg), val));
		}

		if (what == _DIM_CUMULATIVE_MONTH)
			status_list[DIM_CUMULATIVE_MONTH] = v;
		else if (what == REQ_CURRENT_MODE_1 || what == REQ_CURRENT_MODE_2 || what == REQ_CURRENT_MODE_3 ||
				 what == REQ_CURRENT_MODE_4 || what == REQ_CURRENT_MODE_5)
		{
			status_list[DIM_CURRENT] = v;
		}
		else
			status_list[what] = v;

		// Special cases
		if (what == DIM_DAY_GRAPH && num_frame == 10)
		{
			// The cumulative day value must be extracted from graph frames
			// only for previous days, not the current one (for that we have a specific frame)
			if (QDate::currentDate() != getDateFromFrame(msg))
				fillCumulativeDay(status_list, buffer_frame.at(8), msg.frame_open);
		}

		if (what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_MONTH)
		{
			fillYearGraphData(status_list, msg);
			fillMonthlyAverage(status_list, msg);
		}
		emit status_changed(status_list);
	}
}

void EnergyDevice::fillCumulativeDay(StatusList &status_list, QString frame9, QString frame10)
{
	OpenMsg f9(frame9.toStdString());
	int high = f9.whatArgN(3);
	int low = OpenMsg(frame10.toStdString()).whatArgN(1);

	QVariant v;
	v.setValue(EnergyValue(getDateFromFrame(f9), getValue(high, low)));
	status_list[DIM_CUMULATIVE_DAY] = v;
}

void EnergyDevice::fillMonthlyAverage(StatusList &status_list, OpenMsg &msg)
{
	QDate current = QDate::currentDate();
	int average;

	int val = msg.whatArg(0) == "4294967295" ? 0 : msg.whatArgN(0);
	if (static_cast<int>(msg.what()) == _DIM_CUMULATIVE_MONTH)
	{
		int year = msg.whatSubArgN(1) < current.month() ? current.year() : current.year() - 1;
		int total_days = QDate(year, msg.whatSubArgN(1), 1).daysInMonth();
		average = qRound(1.0 * val / total_days);
	}
	else
		average = qRound(1.0 * val / current.day());

	QVariant v_average;
	v_average.setValue(EnergyValue(getDateFromFrame(msg), average));
	status_list[DIM_MONTLY_AVERAGE] = v_average;
}

void EnergyDevice::fillYearGraphData(StatusList &status_list, OpenMsg &msg)
{
	QDate current = QDate::currentDate();
	int index = 12;
	if (static_cast<int>(msg.what()) != DIM_CUMULATIVE_MONTH)
	{
		int month_distance = msg.whatSubArgN(1) - current.month();
		index = month_distance < 0 ? month_distance + 12 : month_distance;
	}
	buffer_year_data[index] = msg.whatArg(0) == "4294967295" ? 0 : msg.whatArgN(0);
	GraphData data;
	data.type = CUMULATIVE_YEAR;
	data.graph = buffer_year_data;
	QVariant v_graph;
	v_graph.setValue(data);
	status_list[DIM_CUMULATIVE_YEAR_GRAPH] = v_graph;
}

void EnergyDevice::parseDailyAverageGraph(const QStringList &buffer_frame, QVariant &v)
{
	QList<int> values_list;
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	data.type = DAILY_AVERAGE;

	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		if (frame_parser.whatArgN(0) == 1)
			continue;

		values_list.append(frame_parser.whatArgN(1));
		values_list.append(frame_parser.whatArgN(2));
		values_list.append(frame_parser.whatArgN(3));
	}

	computeMonthGraphData(values_list, data.graph);

	v.setValue(data);
}

void EnergyDevice::parseCumulativeDayGraph(const QStringList &buffer_frame, QVariant &v)
{
	QList<int> values;
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	data.type = CUMULATIVE_DAY;

	// assume that frames arrive in order
	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		Q_ASSERT_X(frame_parser.whatArgCnt() > 1, "EnergyDevice::parseCumulativeDayGraph", frame_parser.frame_open);
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

	v.setValue(data);
}

void EnergyDevice::parseCumulativeMonthGraph(const QStringList &buffer_frame, QVariant &v)
{
	GraphData data;
	QList<int> values;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	data.type = CUMULATIVE_MONTH;

	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		Q_ASSERT_X(frame_parser.whatArgCnt() > 1, "EnergyDevice::parseCumulativeMonthGraph", frame_parser.frame_open);
		values.append(frame_parser.whatArgN(1));
		values.append(frame_parser.whatArgN(2));
		if (frame_parser.whatArgN(0) != 1)
			values.append(frame_parser.whatArgN(3));
	}
	computeMonthGraphData(values, data.graph);

	v.setValue(data);
}

void EnergyDevice::computeMonthGraphData(const QList<int> &values, QMap<int, int> &graph)
{
	for (int i = 0; i + 1 < values.size(); i += 2)
		graph[i / 2 + 1] = getValue(values[i], values[i + 1]);
}

QDate EnergyDevice::getDateFromFrame(OpenMsg &msg)
{
	int what = msg.what();
	if (what == DIM_DAILY_AVERAGE_GRAPH || what == DIM_CUMULATIVE_MONTH_GRAPH ||
		what == DIM_DAY_GRAPH)
	{
		Q_ASSERT_X(msg.whatSubArgCnt() > 0, "EnergyDevice::getDateFromFrame", msg.frame_open);
		QDate current = QDate::currentDate();
		int month = msg.whatSubArgN(0);
		int day = msg.whatSubArgCnt() > 1 ? msg.whatSubArgN(1) : 1;
		int year = month <= current.month() ? current.year() : current.year() - 1;
		return QDate(year, month, day);
	}
	else if (what == _DIM_CUMULATIVE_MONTH)
	{
		int year = msg.whatSubArgN(0) + 2000;
		int month = msg.whatSubArgN(1);
		return QDate(year, month, 1);
	}
	return QDate::currentDate();
}

float EnergyConversions::convertToRawData(int bt_bus_data, EnergyConversions::EnergyTypology type)
{
	float factor;
	switch (type)
	{
	case DEFAULT_ENERGY:
		factor = 10.;
		break;
	case ELECTRICITY_CURRENT:
		factor = 1000.;
		break;
	}
	return bt_bus_data / factor;
}

float EnergyConversions::convertToMoney(float raw_data, float money_factor)
{
	return raw_data * money_factor;
}
