#include "energy_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>
#include <QList>

#include <assert.h>

const int MAX_VALUE = 255;


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
	REQ_CURRENT_MODE_2 = 1133,
	REQ_CURRENT_MODE_3 = 1129,
	REQ_CURRENT_MODE_4 = -1, // TODO: fake value, to be defined
	REQ_CURRENT_MODE_5 = -1, // TODO: fake value, to be defined
};


EnergyDevice::EnergyDevice(QString where, int _mode) : device(QString("18"), where)
{
	mode = _mode;

	for (int i = 0; i < 12; ++i)
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
	// TODO: per i giorni != dal corrente, e' necessario richiedere il grafico
	// cumulativo giornaliero di quel giorno, e calcolare da quello il totale.
	sendRequest(DIM_CUMULATIVE_DAY);
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
		assert(!"Unknown mode on the energy management!");
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

void EnergyDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || where.toInt() != msg.where())
		return;

	int what = msg.what();

	StatusList status_list;
	QVariant v;

	if (what == DIM_CUMULATIVE_DAY || what == REQ_CURRENT_MODE_1 || what == REQ_CURRENT_MODE_2 ||
		what == REQ_CURRENT_MODE_3 || what == REQ_CURRENT_MODE_4 || what == REQ_CURRENT_MODE_5 ||
		what == DIM_CUMULATIVE_MONTH || what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_YEAR ||
		what == DIM_DAILY_AVERAGE_GRAPH || what == DIM_DAY_GRAPH || what == DIM_CUMULATIVE_MONTH_GRAPH)
	{
		assert(msg.whatArgCnt() > 0);
		qDebug("EnergyDevice::frame_rx_handler -> frame read:%s", frame);
		int num_frame = msg.whatArgN(0);
		// clear the buffer if the first frame of a new graph arrives
		if (num_frame == 1 && msg.whatArgCnt() > 1)
			buffer_frame.clear();

		if (what == DIM_DAILY_AVERAGE_GRAPH)
		{
			// We assume that the frames came in correct (sequential) order
			if (num_frame > 0 && num_frame < 18)
				buffer_frame.append(frame);
			parseDailyAverageGraph(buffer_frame, v);
		}
		else if (what == DIM_DAY_GRAPH)
		{
			if (num_frame > 0 && num_frame < 10)
				buffer_frame.append(frame);
			parseCumulativeDayGraph(buffer_frame, v);
		}
		else if (what == DIM_CUMULATIVE_MONTH_GRAPH)
		{
			if (num_frame > 0 && num_frame < 22)
				buffer_frame.append(frame);
			parseCumulativeMonthGraph(buffer_frame, v);
		}
		else
		{
			int val = msg.whatArgN(0);
			v.setValue(val);
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

		if (what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_MONTH)
		{
			int index = 11;
			if (what != DIM_CUMULATIVE_MONTH)
			{
				int month_distance = msg.whatSubArgN(1) - QDate::currentDate().month();
				index = (month_distance < 0 ? month_distance + 12 : month_distance) - 1;
			}
			qDebug() << "INDEX:" << index << "VAL: " << msg.whatArgN(0);
			buffer_year_data[index] = msg.whatArgN(0);
			GraphData data;
			data.type = CUMULATIVE_YEAR;
			data.graph = buffer_year_data;
			QVariant v;
			v.setValue(data);
			status_list[DIM_CUMULATIVE_YEAR_GRAPH] = v;
		}
		emit status_changed(status_list);
	}
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
		values_list.append(frame_parser.whatArgN(1));
		if (frame_parser.whatArgN(0) != 17)
		{
			values_list.append(frame_parser.whatArgN(2));
			if (frame_parser.whatArgN(0) != 1)
				values_list.append(frame_parser.whatArgN(3));
		}
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
		assert(frame_parser.whatArgCnt() > 1);
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
		assert(frame_parser.whatArgCnt() > 1);
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
	{
		int high = values[i] == MAX_VALUE ? 0 : values[i];
		int low = values[i+1] == MAX_VALUE ? 0 : values[i+1];
		graph[i / 2 + 1] = high * 256 + low;
	}
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
	return raw_data / money_factor;
}
