/*!
 * \banntemperature.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Banner to display zone description and temperature in one row
 *
 * BannTemperature is a banner to display information about not controlled
 * probes in a single row.
 * The class contains both the logic to update the status of the device and
 * the drawing logic.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNTEMPERATURE_H
#define BANNTEMPERATURE_H

#include "banner.h"
#include "main.h"  // TemperatureScale

#include <QString>
#include <QList>

#define DESCR_LABEL_WIDTH 180
#define BORDER_WIDTH        5
#define DESCRIPTION_WIDTH 120
#define TEMPERATURE_WIDTH 120

class device_status;
class device;
class QLabel;


class BannTemperature : public banner
{
Q_OBJECT
public:
	/**
	 * Creates a banner to display temperature information got from
	 * single (not controlled) and external probes.
	 *
	 * \param parent The parent widget
	 * \param name   The widget name
	 * \param config The node in the Dom tree that acts as root of this device
	 * \param dev    The probe (device) connected with this banner
	 */
	BannTemperature(QWidget *parent, QString where, QString descr, device *dev);
	virtual void Draw();
public slots:
	/**
	 * Manages a change in status, like temperature change.
	 *
	 * \param list A list of device_status objects
	 */
	void status_changed(QList<device_status*> sl);
private:
	/// Name of the zone where the probe is installed (read from config)
	QString  probe_descr;
	/// Probed temperature (in celsius)
	int temperature;
	/// Temperature label
	QLabel  *temp_label;
	/// Zone name label
	QLabel  *descr_label;
	TemperatureScale temp_scale;
};

#endif // BANNTEMPERATURE_H

