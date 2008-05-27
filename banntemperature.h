/*!
 * \banntemperature.h
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief 
 *
 * 
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNTEMPERATURE_H
#define BANNTEMPERATURE_H

#include "banner.h"
#include "device.h"
#include "qdom.h"

#define DESCR_LABEL_WIDTH 180
#define BORDER_WIDTH        5
#define DESCRIPTION_WIDTH 120
#define TEMPERATURE_WIDTH 120
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
	BannTemperature(QWidget *parent, const char *name, QDomNode config, device *dev);
	virtual void Draw();
public slots:
	void status_changed(QPtrList<device_status>);
private:
	/**
	 * \brief Dummy function to avoid instantiation 
	 *
	 * BannTemperature has no address so it shouldn't be possible to
	 * instatiate.
	 */
	//virtual void dummy() = 0;
	QDomNode conf_root;
	QString  probe_descr;
	QString  temperature;
	QLabel  *temp_label;
	QLabel  *descr_label;
};

#endif // BANNTEMPERATURE_H

