/*!
 * \banntemperature.h
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
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
#include "device_status.h"
#include "main.h"

#include <qdom.h>

#define DESCR_LABEL_WIDTH 180
#define BORDER_WIDTH        5
#define DESCRIPTION_WIDTH 120
#define TEMPERATURE_WIDTH 120
	
class BtLabelEvo;
class device;

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
	BannTemperature(QWidget *parent, const char *name, QDomNode config, device *dev);
	virtual void Draw();
public slots:
	/**
	 * Manages a change in status, like temperature change.
	 *
	 * \param list A list of device_status objects
	 */
	void status_changed(QPtrList<device_status> list);
private:
	QDomNode conf_root;
	/// Name of the zone where the probe is installed (read from config)
	QString  probe_descr;
	/// Probed temperature
	unsigned  temperature;
	/// Temperature label
	BtLabelEvo  *temp_label;
	/// Zone name label
	BtLabelEvo  *descr_label;
	TemperatureScale temp_scale;
};

#endif // BANNTEMPERATURE_H

