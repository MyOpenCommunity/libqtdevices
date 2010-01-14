/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class can be used to manage a list of temperature label
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef TEMPERATUREVIEWER_H
#define TEMPERATUREVIEWER_H

#include "frame_receiver.h"
#include "main.h" // TemperatureScale
#include "device.h" // StatusList

#include <QObject>
#include <QList>

class Page;
class NonControlledProbeDevice;

class QLabel;
class QString;
class QLCDNumber;


class TemperatureViewer : public QObject
{
Q_OBJECT
public:
	TemperatureViewer(Page *page);
	void add(QString where, int x, int y, int width, int height, QString descr, QString ext);
	void inizializza();

private slots:
	void status_changed(const StatusList &sl);

private:
	Page *linked_page; /// the page in which the temperature objects are displayed.

	struct TemperatureData
	{
		QLCDNumber *lcd;
		QLabel *text;
		NonControlledProbeDevice *device;
	};

	QList<TemperatureData> temp_list;

	/// The temperature scale set in config file (either Celsius or Fahrenheit)
	TemperatureScale temp_scale;

	void updateDisplay(unsigned new_bt_temperature, TemperatureData *temp);
};

#endif // TEMPERATUREVIEWER_H
