/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "energy_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>
#include <QtGlobal>
#include <QList>

const int MAX_VALUE = 255;
const int POLLING_INTERVAL = 10;
const int UPDATE_INTERVAL = 1;
const int STOPPING_TIMEOUT = 5;


namespace
{
	inline int getValue(int high, int low)
	{
		return high == MAX_VALUE && low == MAX_VALUE ? 0 : high * 256 + low;
	}
}

enum RequestDimension
{
	_DIM_CUMULATIVE_MONTH                  = 52, // An implementation detail, ignore this
	_DIM_STATE_UPDATE_INTERVAL             = 1200,   // used to detect start/stop of automatic updates
	_DIM_DAY_GRAPH_16BIT                   = 511,    // used internally, the status list contains DIM_DAY_GRAPH
	_DIM_DAILY_AVERAGE_GRAPH_16BIT         = 512,// used internally, the status list contains DIM_DAILY_AVERAGE_GRAPH
	_DIM_CUMULATIVE_MONTH_GRAPH_32BIT      = 513, // used internally, the status list contains DIM_CUMULATIVE_MONTH_GRAPH
	_DIM_CUMULATIVE_MONTH_GRAPH_PREV_32BIT = 514, // used internally, the status list contains DIM_CUMULATIVE_MONTH_GRAPH
	// TODO change the invalid frame what when it's finalized
	_DIM_INVALID_FRAME = 777,            // if received, it means the device supports auto updates and the
					     // 16/32 bit graph frames

	REQ_DAILY_AVERAGE_GRAPH           = 53,   // graph data for daily average
	REQ_DAILY_AVERAGE_GRAPH_16BIT     = 58,   // graph data for daily average (16bit frames)
	REQ_DAY_GRAPH                     = 52,   // request graph data for a specific day
	REQ_DAY_GRAPH_16BIT               = 57,   // request graph data for a specific day (16bit frames)
	REQ_CUMULATIVE_MONTH_GRAPH        = 56,   // request graph data for cumulative month
	REQ_CUMULATIVE_MONTH_GRAPH_32BIT  = 59,   // request graph data for cumulative month (32 bit frames)
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
	pending_graph_request = 0;
	update_state = UPDATE_IDLE;
	has_new_frames = false;
	update_count = 0;
	update_timer = new QTimer(this);
	connect(update_timer, SIGNAL(timeout()), SLOT(pollingTimeout()));

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
		Q_ASSERT(!"Unknown mode on the energy management!");
	}
	sendRequest(what);
}

void EnergyDevice::sendUpdateStart()
{
	sendFrame(createRequestOpen(who, QString("#1200#%1*%2").arg(mode).arg(UPDATE_INTERVAL), where));
}

void EnergyDevice::sendUpdateStop()
{
	sendFrame(createRequestOpen(who, QString("#1200#%1*%2").arg(mode).arg(0), where));
}

void EnergyDevice::requestCurrentUpdateStart()
{
	update_count += 1;

	if (update_count == 1)
	{
		switch (update_state)
		{
		case UPDATE_IDLE:
			update_state = UPDATE_AUTO;

			if (!has_new_frames)
			{
				requestCurrent();
				update_timer->start(POLLING_INTERVAL * 1000);
			}
			sendUpdateStart();

			break;
		case UPDATE_STOPPING:
			update_state = UPDATE_AUTO;

			break;
		default:
			qFatal("Invalid transition: update_count == 1 and state is UPDATE_AUTO");
			break;
		}
	}
}

void EnergyDevice::requestCurrentUpdateStop()
{
	if (update_count == 0)
		return;
	update_count -= 1;

	if (update_count == 0)
	{
		switch (update_state)
		{
		case UPDATE_AUTO:
			update_state = UPDATE_STOPPING;

			QTimer::singleShot(STOPPING_TIMEOUT * 1000, this, SLOT(stoppingTimeout()));
			break;
		default:
			qFatal("State is UPDATE_STOPPING or UPDATE_IDLE update_count > 0");
			break;
		}
	}
}

void EnergyDevice::pollingTimeout()
{
	requestCurrent();
}

void EnergyDevice::stoppingTimeout()
{
	if (update_state == UPDATE_STOPPING)
	{
		if (update_timer)
			update_timer->stop();
		sendUpdateStop();
		update_state = UPDATE_IDLE;
	}
}

void EnergyDevice::requestCumulativeYear() const
{
	sendRequest(DIM_CUMULATIVE_YEAR);
}

void EnergyDevice::requestDailyAverageGraph(QDate date) const
{
	if (!has_new_frames)
	{
		pending_request_date = date;
		pending_graph_request = REQ_DAILY_AVERAGE_GRAPH;
		requestDailyAverageGraph8Bit(date);
	}
	else
		requestDailyAverageGraph16Bit(date);
}

void EnergyDevice::requestDailyAverageGraph8Bit(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_DAILY_AVERAGE_GRAPH)
		.arg(date.month()), where));
}

void EnergyDevice::requestDailyAverageGraph16Bit(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_DAILY_AVERAGE_GRAPH_16BIT)
		.arg(date.month()), where));
}

void EnergyDevice::requestMontlyAverage(QDate date) const
{
	requestCumulativeMonth(date);
}

void EnergyDevice::requestCumulativeDayGraph(QDate date) const
{
	if (!has_new_frames)
	{
		pending_request_date = date;
		pending_graph_request = REQ_DAY_GRAPH;
		requestCumulativeDayGraph8Bit(date);
	}
	else
		requestCumulativeDayGraph16Bit(date);
}

void EnergyDevice::requestCumulativeDayGraph8Bit(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2#%3").arg(REQ_DAY_GRAPH)
		.arg(date.month()).arg(date.day()), where));
}

void EnergyDevice::requestCumulativeDayGraph16Bit(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2#%3").arg(REQ_DAY_GRAPH_16BIT)
		.arg(date.month()).arg(date.day()), where));
}

void EnergyDevice::requestCumulativeMonthGraph(QDate date) const
{
	if (!has_new_frames)
	{
		pending_request_date = date;
		pending_graph_request = REQ_CUMULATIVE_MONTH_GRAPH;
		requestCumulativeMonthGraph8Bit(date);
	}
	else
		requestCumulativeMonthGraph32Bit(date);
}

void EnergyDevice::requestCumulativeMonthGraph8Bit(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_CUMULATIVE_MONTH_GRAPH)
		.arg(date.month()), where));
}

void EnergyDevice::requestCumulativeMonthGraph32Bit(QDate date) const
{
	sendCompressedFrame(createMsgOpen(who, QString("%1#%2").arg(REQ_CUMULATIVE_MONTH_GRAPH_32BIT)
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
		requestCumulativeMonth(curr.addMonths(i * -1), false); // we compress the request of the graph data
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
		what == DIM_DAILY_AVERAGE_GRAPH || what == DIM_DAY_GRAPH || what == DIM_CUMULATIVE_MONTH_GRAPH ||
		what == _DIM_DAY_GRAPH_16BIT || what == _DIM_DAILY_AVERAGE_GRAPH_16BIT ||
		what == _DIM_CUMULATIVE_MONTH_GRAPH_32BIT || what == _DIM_CUMULATIVE_MONTH_GRAPH_PREV_32BIT)
	{

		// In some cases (when more than a ts is present in the system)
		// a request frame can arrive from the monitor socket. We have to manage this
		// situation.
		if (!msg.whatArgCnt() || msg.IsStateFrame() || msg.IsNormalFrame())
			return;

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
			parseDailyAverageGraph8Bit(buffer_frame, v);
		}
		else if (what == DIM_DAY_GRAPH)
		{
			if (num_frame > 0 && num_frame < 10)
				buffer_frame.append(frame);
			parseCumulativeDayGraph8Bit(buffer_frame, v);
		}
		else if (what == DIM_CUMULATIVE_MONTH_GRAPH)
		{
			if (num_frame > 0 && num_frame < 22)
				buffer_frame.append(frame);
			parseCumulativeMonthGraph8Bit(buffer_frame, v);
		}
		else if (what == _DIM_DAY_GRAPH_16BIT)
		{
			if (num_frame > 0 && num_frame < 25)
				buffer_frame.append(frame);
			parseCumulativeDayGraph16Bit(buffer_frame, v);
		}
		else if (what == _DIM_DAILY_AVERAGE_GRAPH_16BIT)
		{
			if (num_frame > 0 && num_frame < 25)
				buffer_frame.append(frame);
			parseDailyAverageGraph16Bit(buffer_frame, v);
		}
		else if (what == _DIM_CUMULATIVE_MONTH_GRAPH_32BIT ||  what == _DIM_CUMULATIVE_MONTH_GRAPH_PREV_32BIT)
		{
			if (num_frame > 0 && num_frame < 32) // should not be necessary, but just in case
				buffer_frame.append(frame);
			parseCumulativeMonthGraph32Bit(buffer_frame, v, what == _DIM_CUMULATIVE_MONTH_GRAPH_PREV_32BIT);
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
		else if (what == _DIM_DAY_GRAPH_16BIT)
			status_list[DIM_DAY_GRAPH] = v;
		else if (what == _DIM_DAILY_AVERAGE_GRAPH_16BIT)
			status_list[DIM_DAILY_AVERAGE_GRAPH] = v;
		else if (what == _DIM_CUMULATIVE_MONTH_GRAPH_32BIT || what == _DIM_CUMULATIVE_MONTH_GRAPH_PREV_32BIT)
			status_list[DIM_CUMULATIVE_MONTH_GRAPH] = v;
		else
			status_list[what] = v;

		// Special cases
		if (what == DIM_DAY_GRAPH && num_frame == 10)
		{
			// The cumulative day value must be extracted from graph frames
			// only for previous days, not the current one (for that we have a specific frame)
			if (QDate::currentDate() != getDateFromFrame(msg))
				fillCumulativeDay(status_list, buffer_frame.at(8), frame);
		}
		else if (what == _DIM_DAY_GRAPH_16BIT && num_frame == 25)
		{
			// see comment above
			if (QDate::currentDate() != getDateFromFrame(msg))
			{
				QVariant v;
				v.setValue(EnergyValue(getDateFromFrame(msg), msg.whatArgN(1)));
				status_list[DIM_CUMULATIVE_DAY] = v;
			}
		}
		else if (what == _DIM_DAILY_AVERAGE_GRAPH_16BIT && num_frame == 25)
		{
			QVariant v;
			v.setValue(EnergyValue(getDateFromFrame(msg), msg.whatArgN(1)));
			status_list[DIM_MONTLY_AVERAGE] = v;
		}

		if (what == _DIM_CUMULATIVE_MONTH || what == DIM_CUMULATIVE_MONTH)
		{
			fillYearGraphData(status_list, msg);
			fillMonthlyAverage(status_list, msg);
		}

		emit status_changed(status_list);
	}

	if (what == _DIM_STATE_UPDATE_INTERVAL)
	{
		if (msg.whatArgCnt() != 1)
			return;

		handleAutomaticUpdate(status_list, msg);
	}

	if (what == _DIM_INVALID_FRAME)
	{
		// switch the flag for old/new frame support and resend
		// the auto update request if needed
		setHasNewFrames(update_timer->isActive());
	}
}

void EnergyDevice::setHasNewFrames(bool restart_update_requests)
{
	has_new_frames = true;

	// delete the polling timer and send the frame to request
	// automatic updates
	update_timer->stop();
	update_timer->deleteLater();
	update_timer = NULL;

	// this might send one unneeded frame, but removes the need for
	// an additional state
	if (restart_update_requests)
		sendUpdateStart();
	else if (update_state == UPDATE_STOPPING)
		// to force resending the start frame if restarted
		update_state = UPDATE_IDLE;

	// resend the last graph request using 16/32 bit frames
	switch (pending_graph_request)
	{
	case REQ_DAILY_AVERAGE_GRAPH:
		requestDailyAverageGraph(pending_request_date);
		break;
	case REQ_DAY_GRAPH:
		requestCumulativeDayGraph(pending_request_date);
		break;
	case REQ_CUMULATIVE_MONTH_GRAPH:
		requestCumulativeMonthGraph(pending_request_date);
		break;
	};

	pending_graph_request = 0;
}

void EnergyDevice::handleAutomaticUpdate(StatusList &status_list, OpenMsg &msg)
{
	int time = msg.whatArgN(0);

	if (!has_new_frames)
	{
		qDebug("Switching from polling mode to auto-update mode");

		setHasNewFrames(update_state == UPDATE_AUTO && time != 0);
	}

	// at this point need_polling is always false
	if (time == 0)
	{
		qDebug("Received auto-update stop frame");

		switch (update_state)
		{
		case UPDATE_AUTO:
			// restart automatic updates since we need them
			sendUpdateStart();

			break;
		case UPDATE_STOPPING:
			// to force resending the start frame if restarted
			update_state = UPDATE_IDLE;

			break;
		default:
			// no handling needed for UPDATE_IDLE
			break;
		}
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

void EnergyDevice::parseDailyAverageGraph8Bit(const QStringList &buffer_frame, QVariant &v)
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

	computeMonthGraphData(data.date.daysInMonth(), values_list, data.graph);

	v.setValue(data);
}

void EnergyDevice::parseDailyAverageGraph16Bit(const QStringList &buffer_frame, QVariant &v)
{
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	data.type = DAILY_AVERAGE;

	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		Q_ASSERT_X(frame_parser.whatArgCnt() > 1, "EnergyDevice::parseDailyAverageGraph", frame_parser.frame_open);

		data.graph[i + 1] = frame_parser.whatArgN(1);
	}

	v.setValue(data);
}

void EnergyDevice::parseCumulativeDayGraph8Bit(const QStringList &buffer_frame, QVariant &v)
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

void EnergyDevice::parseCumulativeDayGraph16Bit(const QStringList &buffer_frame, QVariant &v)
{
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	data.date = getDateFromFrame(tmp);
	data.type = CUMULATIVE_DAY;

	for (int i = 0; i < buffer_frame.size(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		Q_ASSERT_X(frame_parser.whatArgCnt() > 1, "EnergyDevice::parseCumulativeDayGraph", frame_parser.frame_open);

		data.graph[i + 1] = frame_parser.whatArgN(1);
	}

	v.setValue(data);
}

void EnergyDevice::parseCumulativeMonthGraph8Bit(const QStringList &buffer_frame, QVariant &v)
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
	computeMonthGraphData(data.date.daysInMonth(), values, data.graph);

	v.setValue(data);
}

void EnergyDevice::parseCumulativeMonthGraph32Bit(const QStringList &buffer_frame, QVariant &v, bool prev_year)
{
	GraphData data;
	OpenMsg tmp(buffer_frame[0].toStdString());
	QDate d = getDateFromFrame(tmp);
	data.type = CUMULATIVE_MONTH;

	if (prev_year)
		data.date.setYMD(d.year() - 1, d.month(), d.day());
	else
		data.date = d;

	for (int i = 0; i < buffer_frame.size() && i < data.date.daysInMonth(); ++i)
	{
		OpenMsg frame_parser(buffer_frame[i].toStdString());
		Q_ASSERT_X(frame_parser.whatArgCnt() > 1, "EnergyDevice::parseCumulativeMonthGraph", frame_parser.frame_open);

		data.graph[i + 1] = frame_parser.whatArgN(1);
	}

	v.setValue(data);
}

void EnergyDevice::computeMonthGraphData(int days_in_month, const QList<int> &values, QMap<int, int> &graph)
{
	for (int i = 0; i + 1 < values.size(); i += 2)
	{
		if (i / 2 + 1 > days_in_month)
			break;
		graph[i / 2 + 1] = getValue(values[i], values[i + 1]);
	}
}

QDate EnergyDevice::getDateFromFrame(OpenMsg &msg)
{
	int what = msg.what();
	if (what == DIM_DAILY_AVERAGE_GRAPH || what == DIM_CUMULATIVE_MONTH_GRAPH ||
		what == DIM_DAY_GRAPH || what == _DIM_DAY_GRAPH_16BIT ||
		what == _DIM_DAILY_AVERAGE_GRAPH_16BIT || what == _DIM_CUMULATIVE_MONTH_GRAPH_32BIT ||
		what == _DIM_CUMULATIVE_MONTH_GRAPH_PREV_32BIT)
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
	case OTHER_ENERGY:
		factor = 1.;
		break;
	}
	return bt_bus_data / factor;
}

float EnergyConversions::convertToMoney(float raw_data, float money_factor)
{
	return raw_data * money_factor;
}
