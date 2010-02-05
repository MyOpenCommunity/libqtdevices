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

	virtual void manageFrame(OpenMsg &msg);

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


private:

	// EnergyDevice doesn't use the sendRequest of the device (because it should use the compression
	// of the frames) but instead it defines its own version.
	void sendRequest(int what) const;
	void sendRequest(QString what) const;
	void parseCumulativeDayGraph(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeMonthGraph(const QStringList &buffer_frame, QVariant &v);
	void parseDailyAverageGraph(const QStringList &buffer_frame, QVariant &v);
	void computeMonthGraphData(int days_in_month, const QList<int> &values, QMap<int, int> &graph);

	// The following fill* methods are special methods that handle the case when a single
	// value or a graph value needs to be calculated from other values.
	void fillYearGraphData(StatusList &status_list, OpenMsg &msg);
	void fillMonthlyAverage(StatusList &status_list, OpenMsg &msg);
	void fillCumulativeDay(StatusList &status_list, QString frame9, QString frame10);
	void handleAutomaticUpdate(StatusList &status_list, OpenMsg &msg);

	// frames for automatic updates
	void sendUpdateStart();
	void sendUpdateStop();

	void setPollingOff();

	QDate getDateFromFrame(OpenMsg &msg);

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

	UpdateState update_state;

	// true if the device does not support automatic updates
	bool need_polling;

	// number of pages/banners that have requested automatic updates
	int update_count;

	// timer used to request updates when automatic updates are active
	// but the device does not support automatic update frames
	QTimer *update_timer;

	mutable QStringList buffer_frame;
	int mode;

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

