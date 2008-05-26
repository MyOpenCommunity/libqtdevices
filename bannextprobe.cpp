/*!
 * \bannextprobe.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief 
 *
 * 
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "bannextprobe.h"
#include "device_cache.h"

BannExtProbe::BannExtProbe(QWidget *parent, const char *name, QDomNode n)
	: BannTemperature(parent, name, n)
{
	device *dev = btouch_device_cache.get_temperature_probe(getAddress(), true);
	QObject::connect(dev, SIGNAL(status_changed(QPtrList<device_status>)),
			SLOT(status_changed(QPtrList<device_status>)));
}
