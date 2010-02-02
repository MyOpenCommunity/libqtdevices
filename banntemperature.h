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

class NonControlledProbeDevice;
class QLabel;
typedef QHash<int, QVariant> StatusList;


class BannTemperature : public BannerOld
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
	BannTemperature(QWidget *parent, QString where, QString descr, NonControlledProbeDevice *dev);

	virtual void inizializza(bool forza=false);

public slots:
	/**
	 * Manages a change in status, like temperature change.
	 */
	void status_changed(const StatusList &sl);
private:
	void setTemperature();
	/// Probed temperature (arrives from frame_interpreter directly in celsius, not in bt)
	int temperature;
	/// Temperature label
	QLabel  *temp_label;
	TemperatureScale temp_scale;
	NonControlledProbeDevice *dev;
};

#endif // BANNTEMPERATURE_H

