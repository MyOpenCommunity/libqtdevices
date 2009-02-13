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

#include <QMap>
#include <QVariant>

class QDate;
class OpenMsg;


typedef QMap<int, int> GraphData;
Q_DECLARE_METATYPE(GraphData)


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
	void requestDayGraph(QDate date) const;
	void requestCumulativeMonthGraph(QDate date) const;

	enum Type
	{
		DIM_CUMULATIVE_YEAR   = 51,
		DIM_CUMULATIVE_MONTH  = 53,
		DIM_CUMULATIVE_DAY    = 54,
		DIM_CURRENT           = 113,

		ANS_DAILY_AVERAGE_GRAPH      = 57,    // read graph data for cumulative daily average
		ANS_DAY_GRAPH                = 56,    // read graph data for a specific day
		ANS_CUMULATIVE_MONTH_GRAPH   = 510,   // read graph data for cumulative month
	};

public slots:
	void frame_rx_handler(char *frame);

private:
	void sendRequest(int what) const;
	void sendRequest(QString what) const;
	QVariant parseDayGraph(const QList<QString> &buffer_frame, OpenMsg &msg);
	QVariant parseCumulativeMonthGraph(const QList<QString> &buffer_frame);
	static const int MAX_VALUE = 255;
	mutable QList<QString> buffer_frame;
};

#endif // ENERGY_DEVICE_H

