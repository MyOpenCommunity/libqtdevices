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

#include <QHash>
#include <QVariant>

class QDate;


typedef QHash<int, int> GraphData;
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

	enum Type
	{
		DIM_CUMULATIVE_DAY    = 54,
		DIM_CURRENT           = 113,
		DIM_CUMULATIVE_MONTH  = 53,
		_DIM_CUMULATIVE_MONTH = 52, // An implementation detail, ignore this
		DIM_CUMULATIVE_YEAR   = 51,
		DIM_DAILY_AVERAGE     = 57
	};

public slots:
	void frame_rx_handler(char *frame);

private:
	void sendRequest(int what) const;
	void sendRequest(QString what) const;
	mutable QList<QString> buffer_frame;
};

#endif // ENERGY_DEVICE_H

