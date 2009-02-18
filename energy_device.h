/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the specific device for energy management.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef ENERGY_DEVICE_H
#define ENERGY_DEVICE_H

#include "device.h"

#include <QDate>
#include <QMap>
#include <QStringList>
#include <QVariant>

class OpenMsg;

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
	EnergyDevice(QString where);

	// The request methods, used to request an information
	void requestCumulativeDay() const;
	void requestCurrent() const;
	void requestCumulativeMonth(QDate date) const;
	void requestCumulativeYear() const;
	void requestDailyAverageGraph(QDate date) const;
	void requestCumulativeDayGraph(QDate date) const;
	void requestCumulativeMonthGraph(QDate date) const;

	enum Type
	{
		DIM_CUMULATIVE_YEAR   = 51,
		DIM_CUMULATIVE_MONTH  = 53,
		DIM_CUMULATIVE_DAY    = 54,
		DIM_CURRENT           = 113,

		DIM_DAILY_AVERAGE_GRAPH      = 57,    // read graph data for cumulative daily average
		DIM_DAY_GRAPH                = 56,    // read graph data for a specific day
		DIM_CUMULATIVE_MONTH_GRAPH   = 510,   // read graph data for cumulative month
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
	void sendRequest(int what) const;
	void sendRequest(QString what) const;
	void parseCumulativeDayGraph(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeMonthGraph(const QStringList &buffer_frame, QVariant &v);
	void parseDailyAverageGraph(const QStringList &buffer_frame, QVariant &v);
	void computeMonthGraphData(const QList<int> &values, QMap<int, int> &graph);
	QDate getDateFromFrame(OpenMsg &msg);
	static const int MAX_VALUE = 255;
	mutable QStringList buffer_frame;
};


struct GraphData
{
	GraphData() : type(EnergyDevice::CUMULATIVE_DAY) { }
	QMap<int, int> graph;
	QDate date;
	EnergyDevice::GraphType type;
	bool operator==(const GraphData &other)
	{
		return (graph == other.graph) && (date == other.date) &&
			(type == other.type);
	}
};
Q_DECLARE_METATYPE(GraphData)

#endif // ENERGY_DEVICE_H

