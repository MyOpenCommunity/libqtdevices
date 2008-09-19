/*!
 * \banntemperature.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "banntemperature.h"
#include "bannpuls.h"
#include "fontmanager.h"
#include "device.h"
#include "main.h"
#include "btlabelevo.h"
#include "scaleconversion.h"

BannTemperature::BannTemperature(QWidget *parent, const char *name, QDomNode config, device *dev)
	: banner(parent, name)
{
	conf_root = config;
	probe_descr = conf_root.namedItem("descr").toElement().text();
	temperature = 1235;
	temp_scale = readTemperatureScale();

	setChi("4");

	QDomNode addr = conf_root.namedItem("where");
	if (!addr.isNull())
		setAddress(addr.toElement().text().ascii());
	else
		qFatal("[TERMO] obj: %s, no where in configuration", probe_descr.ascii());

	descr_label = new BtLabelEvo(this);
	descr_label->setGeometry(BORDER_WIDTH, 0, DESCRIPTION_WIDTH, BANPULS_ICON_DIM_Y);
	temp_label = new BtLabelEvo(this);
	temp_label->setGeometry(DESCRIPTION_WIDTH, 0, TEMPERATURE_WIDTH, BANPULS_ICON_DIM_Y);

	QObject::connect(dev, SIGNAL(status_changed(QPtrList<device_status>)),
			SLOT(status_changed(QPtrList<device_status>)));
}

void BannTemperature::status_changed(QPtrList<device_status> list)
{
	QPtrListIterator<device_status> it (list);
	device_status *dev;
	bool update = false;

	while ((dev = it.current()) != 0)
	{
		++it;
		if (dev->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			dev->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			temperature = curr_temp.get_val();
			update = true;
		}
	}

	if (update)
		Draw();
}

void BannTemperature::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(AlignAuto | AlignVCenter);
	descr_label->setText(probe_descr);

	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(AlignCenter);
	switch (temp_scale)
	{
		case CELSIUS:
			temp_label->setText(celsiusString(bt2Celsius(temperature)));
			break;
		case FAHRENHEIT:
			temp_label->setText(fahrenheitString(bt2Fahrenheit(temperature)));
			break;
		default:
			qWarning("BannTemperature: unknown scale");
	}
	banner::Draw();
}
