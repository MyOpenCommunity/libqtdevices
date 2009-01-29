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
#include "bann1_button.h" // bannPuls
#include "fontmanager.h" // bt_global::font
#include "device.h"
#include "device_status.h"
#include "main.h" // bt_global::config
#include "scaleconversion.h"

#include <QLabel>
#include <QFont>


BannTemperature::BannTemperature(QWidget *parent, QString where, QString descr, device *dev) : banner(parent)
{
	probe_descr = descr;
	temperature = -235;
	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());

	if (!where.isNull())
		setAddress(where);

	descr_label = new QLabel(this);
	descr_label->setGeometry(BORDER_WIDTH, 0, DESCRIPTION_WIDTH, BANPULS_ICON_DIM_Y);
	temp_label = new QLabel(this);
	temp_label->setGeometry(DESCRIPTION_WIDTH, 0, TEMPERATURE_WIDTH, BANPULS_ICON_DIM_Y);

	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			SLOT(status_changed(QList<device_status*>)));
}

void BannTemperature::status_changed(QList<device_status*> sl)
{
	bool update = false;

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			temperature = curr_temp.get_val();
			update = true;
		}
	}

	if (update)
		Draw();
}

void BannTemperature::Draw()
{
	descr_label->setFont(bt_global::font.get(FontManager::SUBTITLE));
	descr_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	descr_label->setText(probe_descr);

	temp_label->setFont(bt_global::font.get(FontManager::SUBTITLE));
	temp_label->setAlignment(Qt::AlignCenter);
	switch (temp_scale)
	{
		case CELSIUS:
			temp_label->setText(celsiusString(temperature));
			break;
		case FAHRENHEIT:
			temp_label->setText(fahrenheitString(celsius2Bt(temperature)));
			break;
		default:
			qWarning("BannTemperature: unknown scale");
	}
	banner::Draw();
}
