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


#ifndef ENERGY_DEVICE_H
#define ENERGY_DEVICE_H

#include "device.h"

#include <QDate>
#include <QMap>
#include <QStringList>
#include <QVariant>

class OpenMsg;

namespace EnergyConversions
{
	enum EnergyTypology
	{
		DEFAULT_ENERGY,            // default conversion, divide number by 10
		ELECTRICITY_CURRENT,       // specific conversion for current electricity
		OTHER_ENERGY,              // conversion for other current energy value
	};

	float convertToRawData(int bt_bus_data, EnergyTypology type = DEFAULT_ENERGY);
	float convertToMoney(float raw_data, float money_factor);
}


class AutomaticUpdates : public QObject
{
friend class TestEnergyDevice;
Q_OBJECT
public:
	AutomaticUpdates(QString where, int _mode);

	void requestCurrent() const;
	void requestCurrentUpdateStart();
	void requestCurrentUpdateStop();

	// not part of the public interface, but used by the containing device
	void setHasNewFrames();
	void handleAutomaticUpdate(OpenMsg &msg);

private:
	void sendUpdateStart();
	void sendUpdateStop();
	void setHasNewFrames(bool restart_update_requests);

private slots:
	void pollingTimeout();
	void stoppingTimeout();

private:
	enum UpdateState
	{
		// automatic updates are disabled
		UPDATE_IDLE,
		// automatic updates are enabled
		UPDATE_AUTO,
		// automatic updates will be stopped after a timeout expires
		UPDATE_STOPPING,
	};

	// true if the device supports automatic updates without polling
	// and the new 16/32 bit frames for graphs
	bool has_new_frames;

	// status of automatic updates
	UpdateState update_state;

	// number of pages/banners that have requested automatic updates
	int update_count;

	// timer used to request updates when automatic updates are active
	// but the device does not support automatic update frames
	QTimer *update_timer;

	int mode;

	QString where;
	device *dev;
};


/**
 * This class parses the incoming frames for who = 18 (Energy Management) and sends
 * updates to widgets through status_changed() signal.
 * The main assumptions on the incoming frames are that:
 *  - the frames for a graph arrive in order;
 *  - if a graph frame stream is interrupted, no other frames for that stream will arrive later
 *      (ie, there can't be 'holes' in the stream);
 *  - if a stream is interrupted and the last value of the last frame is the high byte
 *      (ie, we can't reconstruct the complete value), nothing must be visualized for that value.
 */
class EnergyDevice : public device
{
friend class TestEnergyDevice;
Q_OBJECT
public:
	EnergyDevice(QString where, int _mode);

	// The request methods, used to request an information
	void requestCumulativeDay(QDate date) const;
	void requestCurrent() const;
	void requestCumulativeMonth(QDate date) const;
	void requestCumulativeYear() const;
	void requestDailyAverageGraph(QDate date) const;
	void requestMontlyAverage(QDate date) const;
	void requestCumulativeDayGraph(QDate date) const;
	void requestCumulativeMonthGraph(QDate date) const;
	void requestCumulativeYearGraph() const;
	void requestCurrentUpdateStart();
	void requestCurrentUpdateStop();

	enum Type
	{
		DIM_CUMULATIVE_YEAR   = 51,
		DIM_CUMULATIVE_MONTH  = 53,
		DIM_CUMULATIVE_DAY    = 54,
		// The value of DIM_CURRENT doesn't matter, because the real value depends on mode
		// (see the implementation of the requestCurrent method for more detail)
		DIM_CURRENT,
		DIM_DAILY_AVERAGE_GRAPH      = 57,    // read graph data for cumulative daily average
		DIM_DAY_GRAPH                = 56,    // read graph data for a specific day
		DIM_CUMULATIVE_MONTH_GRAPH   = 510,   // read graph data for cumulative month
		DIM_CUMULATIVE_YEAR_GRAPH,  // read graph data for comulative year (as DIM_CURRENT, the value doesn't matter)
		DIM_MONTLY_AVERAGE, // as DIM_CURRENT, the value doesn't matter
	};

	enum GraphType
	{
		CUMULATIVE_DAY,
		CUMULATIVE_MONTH,
		CUMULATIVE_YEAR,
		DAILY_AVERAGE
	};

public slots:
	void frame_rx_handler(char *frame);

private:
	void requestCumulativeMonth(QDate date, bool use_compressed_init) const;

	void requestCumulativeDayGraph8Bit(QDate date) const;
	void requestCumulativeDayGraph16Bit(QDate date) const;
	void requestCumulativeMonthGraph8Bit(QDate date) const;
	void requestCumulativeMonthGraph32Bit(QDate date) const;
	void requestDailyAverageGraph8Bit(QDate date) const;
	void requestDailyAverageGraph16Bit(QDate date) const;

	// EnergyDevice doesn't use the sendRequest of the device (because it should use the compression
	// of the frames) but instead it defines its own version.
	void sendRequest(int what, bool use_compressed_init=false) const;
	void sendRequest(QString what, bool use_compressed_init=false) const;
	void parseCumulativeDayGraph8Bit(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeDayGraph16Bit(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeMonthGraph8Bit(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeMonthGraph32Bit(const QStringList &buffer_frame, QVariant &v, bool prev_year);
	void parseDailyAverageGraph8Bit(const QStringList &buffer_frame, QVariant &v);
	void parseDailyAverageGraph16Bit(const QStringList &buffer_frame, QVariant &v);
	void computeMonthGraphData(int days_in_month, const QList<int> &values, QMap<int, int> &graph);

	// The following fill* methods are special methods that handle the case when a single
	// value or a graph value needs to be calculated from other values.
	void fillYearGraphData(StatusList &status_list, OpenMsg &msg);
	void fillMonthlyAverage(StatusList &status_list, OpenMsg &msg);
	void fillCumulativeDay(StatusList &status_list, QString frame9, QString frame10);

	void setHasNewFrames();

	QDate getDateFromFrame(OpenMsg &msg);

private:
	// handle automatic updates of current measure
	AutomaticUpdates current_updates;

	// true if the device supports automatic updates without polling
	// and the new 16/32 bit frames for graphs
	bool has_new_frames;

	// last graph request
	mutable int pending_graph_request;
	mutable QDate pending_request_date;

	mutable QStringList buffer_frame;

	QMap<int, int> buffer_year_data; // a buffer used to store the graph data
};


struct GraphData
{
	GraphData() : type(EnergyDevice::CUMULATIVE_DAY) { }
	QMap<int, int> graph;
	QDate date;
	EnergyDevice::GraphType type;
	bool operator==(const GraphData &other) const
	{
		return (graph == other.graph) && (date == other.date) &&
			(type == other.type);
	}
};
Q_DECLARE_METATYPE(GraphData)


// To use the QPair inside the status_changed we have to register the
// QPair as a type known to QMetaType. We have also define a typedef
// to avoid the limitation of the macro, that cannot handle correctly commas
// inside its arguments.
typedef QPair<QDate, int> EnergyValue;
Q_DECLARE_METATYPE(EnergyValue);

#endif // ENERGY_DEVICE_H

