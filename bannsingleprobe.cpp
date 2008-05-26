/*!
 * \bannextprobe.cpp
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

#include "bannsingleprobe.h"
#include "device_cache.h"

BannSingleProbe::BannSingleProbe(QWidget *parent, const char *name, QDomNode n)
	: BannTemperature(parent, name, n)
{
	QDomNode addr = n.namedItem("where");
	setAddress(addr.toElement().text().ascii());
	setChi("4");
	device *dev = btouch_device_cache.get_temperature_probe(getAddress(), false);
	QObject::connect(dev, SIGNAL(status_changed(QPtrList<device_status>)),
			SLOT(status_changed(QPtrList<device_status>)));
}

