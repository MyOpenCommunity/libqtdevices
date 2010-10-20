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


/*!
	\ingroup EnergyData
	\brief Functions to convert energy values for display.
*/
namespace EnergyConversions
{
	/*!
		\brief The type of graph in status notifications.

		\sa convertToMoney()
	*/
	enum EnergyTypology
	{
		ELECTRICITY,   /*!< Conversion factor for electricity. */
		OTHER_ENERGY,  /*!< Conversion factor for all other energy types. */
	};

	/*!
		\brief Converts a measure to the units used for display.
	*/
	double convertToRawData(qint64 bt_bus_data, EnergyTypology type);


	/*!
		\brief Converts from the units used for display to the corresponding cost.

		\sa EnergyRate
	*/
	double convertToMoney(double raw_data, float money_factor);
}


// Internal helper class used to handle automatic updates.
class AutomaticUpdates : public QObject
{
friend class TestEnergyDevice;
Q_OBJECT
public:
	AutomaticUpdates(QString where, int _mode, device *_dev);

	void requestCurrent() const;
	void requestCurrentUpdate();
	void requestCurrentUpdateStart();
	void requestCurrentUpdateStop();
	void flushCurrentUpdateStop();

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


/*!
	\ingroup EnergyData
	\brief A device to read energy/water/gas/heating consumption data.

	Consumption values are expressed using different using dipending on the energy type:
	\li Electricity: watt
	\li Water: liters
	\li Gas: dm3 (liters)
	\li Hot water: calories
	\li Heating/cooling: calories

	\section EnergyDevice-dimensions Dimensions
	\startdim
	\dim{DIM_CURRENT,::EnergyValue,,Instantaneous consumption.}
	\dim{DIM_CUMULATIVE_YEAR,::EnergyValue,,Total consumption over the last year (current month and the previous 11 months).}
	\dim{DIM_CUMULATIVE_MONTH,::EnergyValue,,Total consumption over a month.}
	\dim{DIM_CUMULATIVE_DAY,::EnergyValue,,Total consumption over a day.}
	\dim{DIM_DAILY_AVERAGE_GRAPH,GraphData,,Average consumption for each hour in a day (computed over a month).}
	\dim{DIM_DAY_GRAPH,GraphData,,Consumption for each hour in a day.}
	\dim{DIM_CUMULATIVE_MONTH_GRAPH,GraphData,,Total consumption for each day in a month.}
	\dim{DIM_CUMULATIVE_YEAR_GRAPH,GraphData,,Total consumption for each month in the last year (current month and the previous 11 months)}
	\dim{DIM_MONTLY_AVERAGE,::EnergyValue,,Average consumption value for the days in a month.}
	\enddim

	After requesting a status update for a specific measure, it is not unusual that
	status updates for multiple measures will be delivered; the exact number and type
	of status updates depends on the physical device model.

	\section FrameOrdering Frame ordering problems

	\a requestCumulativeMonthGraph() must be the last in a sequence of request frames,
	otherwise the physical device might not send back all the requested data (depending
	on the device model).
*/
class EnergyDevice : public device
{
friend class TestEnergyDevice;
Q_OBJECT
public:
	/*!
		\brief Constructor

		Build an energy device using the given \a where and \a mode, where \a mode
		is the measure type and can be:
		\li 1: electricity
		\li 2: water
		\li 3: gas
		\li 4: hot water
		\li 5: heating/conditioning
	*/
	EnergyDevice(QString where, int mode);

	/*!
		\brief Request total consumption over a day (DIM_CUMULATIVE_DAY).
	*/
	void requestCumulativeDay(QDate date) const;

	/*!
		\brief Request the instantaneous consumption.
	*/
	void requestCurrent() const;

	/*!
		\brief Request total consumption over a month (DIM_CUMULATIVE_MONTH).
	*/
	void requestCumulativeMonth(QDate date) const;

	/*!
		\brief Request total consumption over the last 12 months (DIM_CUMULATIVE_YEAR).
	*/
	void requestCumulativeYear() const;

	/*!
		\brief Request graph of average consumption for each hour in a day (DIM_DAILY_AVERAGE_GRAPH).
	*/
	void requestDailyAverageGraph(QDate date) const;

	/*!
		\brief Request average consumption value for the days in a month (DIM_MONTLY_AVERAGE).
	*/
	void requestMontlyAverage(QDate date) const;

	/*!
		\brief Request graph of consumption for each hour in a day (DIM_DAY_GRAPH).
	*/
	void requestCumulativeDayGraph(QDate date) const;

	/*!
		\brief Request graph of total consumption for each day in a month and the
		graph of average consumption for each hour in a day (DIM_CUMULATIVE_MONTH_GRAPH,
		DIM_DAILY_AVERAGE_GRAPH).
	*/
	void requestCumulativeMonthGraph(QDate date) const;

	/*!
		\brief Request total consumption over the last 12 months (DIM_CUMULATIVE_YEAR).
	*/
	void requestCumulativeYearGraph() const;

	/*!
		\brief Request the instantaneous consumption and (for some actuator types)
		the start of periodic updates.
	*/
	void requestCurrentUpdate();

	/*!
		\brief Request automatic updates for the instantaneous consumption.

		After this function is called the device starts sending automatic updates
		for the instantaneous consumption.

		If requestCurrentUpdateStart() is called multiple times (for example by
		different graphic items) the automatic updates stop only after
		requestCurrentUpdateStop() is called the same number of times.
	*/
	void requestCurrentUpdateStart();

	/*!
		\brief Stop automatic updates for the instantaneous consumption.

		Note that, to avoid excessive traffic on the SCS bus in case of repeated
		calls to requestCurrentUpdateStart()/ requestCurrentUpdateStop(),
		automatic upates are stopped about 100 msec after this function is called.

		\sa requestCurrentUpdateStart()
	*/
	void requestCurrentUpdateStop();

	/*!
		\brief Stop automatic updates directly.

		Called after requestCurrentUpdateStop(), forces the update stop to be processed
		immedialty.  This is only required to avoid some frame ordering problems.

		\sa requestCurrentUpdateStop()
		\sa \ref FrameOrdering
	*/
	void flushCurrentUpdateStop();

	/*!
		\refdim{EnergyDevice}
	*/
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

	/*!
		\brief The type of graph in status notifications.
	*/
	enum GraphType
	{
		CUMULATIVE_DAY,   /*!< Consumption for each hour in a day. */
		CUMULATIVE_MONTH, /*!< Total consumption for each day in a month. */
		CUMULATIVE_YEAR,  /*!< Total consumption for each month in a year. */
		DAILY_AVERAGE     /*!< Average consumption for each hour in a day (computed over a month). */
	};

protected:
	bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	void requestCumulativeDayGraph8Bit(QDate date) const;
	void requestCumulativeDayGraph16Bit(QDate date) const;
	void requestCumulativeMonthGraph8Bit(QDate date) const;
	void requestCumulativeMonthGraph32Bit(QDate date) const;
	void requestDailyAverageGraph8Bit(QDate date) const;
	void requestDailyAverageGraph16Bit(QDate date) const;

	void sendRequest(int what) const;
	void sendRequest(QString what) const;
	void sendInit(QString frame) const;

	void parseCumulativeDayGraph8Bit(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeDayGraph16Bit(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeMonthGraph8Bit(const QStringList &buffer_frame, QVariant &v);
	void parseCumulativeMonthGraph32Bit(const QStringList &buffer_frame, QVariant &v, bool prev_year);
	void parseDailyAverageGraph8Bit(const QStringList &buffer_frame, QVariant &v);
	void parseDailyAverageGraph16Bit(const QStringList &buffer_frame, QVariant &v);
	void computeMonthGraphData(int days_in_month, const QList<int> &values, QMap<int, unsigned int> &graph);

	// The following fill* methods are special methods that handle the case when a single
	// value or a graph value needs to be calculated from other values.
	void fillYearGraphData(DeviceValues &values_list, OpenMsg &msg);
	void fillYearTotalData(DeviceValues &values_list, OpenMsg &msg);
	void fillMonthlyAverage(DeviceValues &values_list, OpenMsg &msg);
	void fillCumulativeDay(DeviceValues &values_list, QString frame9, QString frame10);

	void setHasNewFrames();

	QDate getDateFromFrame(OpenMsg &msg);

private:
	// scaling factor to be applied to data from 8 bit graph frames so they
	// have the same scale as current measure and 16/32 bit graph frames
	int scaling_factor_old_frames;

	// handle automatic updates of current measure
	AutomaticUpdates current_updates;

	// true if the device supports automatic updates without polling
	// and the new 16/32 bit frames for graphs
	bool has_new_frames;

	// last graph request
	mutable int pending_graph_request;
	mutable QDate pending_request_date;

	mutable QStringList buffer_frame;

	QMap<int, unsigned int> buffer_year_data; // a buffer used to store the graph data
};




/*!
	\ingroup EnergyData
	\brief Contains multiple values composing a graph.
*/
struct GraphData
{
	GraphData() : type(EnergyDevice::CUMULATIVE_DAY) { }

	/*!
		\brief The graph data.

		The keys always start at 1
		\li EnergyDevice::DAILY_AVERAGE, EnergyDevice::CUMULATIVE_DAY: the key is the hour of the day
		\li EnergyDevice::CUMULATIVE_MONTH: the key is the day of the month
		\li EnergyDevice::CUMULATIVE_YEAR: key 12 contains the value for the current month, lower keys contain the value for previous months
	*/
	QMap<int, unsigned int> graph;

	/*!
		\brief The date the data refers to.

		The interpretation varies depending on graph data type
		\li EnergyDevice::DAILY_AVERAGE, EnergyDevice::CUMULATIVE_DAY: the day the data refers to.
		\li EnergyDevice::CUMULATIVE_MONTH: teh first day of the month the data refers to.
		\li EnergyDevice::CUMULATIVE_YEAR: not relevant.
	*/
	QDate date;

	/*!
		\brief Type of data contained in the graph.

		\sa EnergyDevice
	*/
	EnergyDevice::GraphType type;
	bool operator==(const GraphData &other) const
	{
		return (graph == other.graph) && (date == other.date) &&
			(type == other.type);
	}
};
Q_DECLARE_METATYPE(GraphData)


// To use the QPair inside the valueReceived we have to register the
// QPair as a type known to QMetaType. We have also define a typedef
// to avoid the limitation of the macro, that cannot handle correctly commas
// inside its arguments.
/*!
	\ingroup EnergyData
	\brief Consumption value for the given date.

	\sa EnergyDevice
*/
typedef QPair<QDate, qint64> EnergyValue;
Q_DECLARE_METATYPE(EnergyValue);

#endif // ENERGY_DEVICE_H

/*! \file */ /* otherwise doxygen does not generate documentation for typedefs */

