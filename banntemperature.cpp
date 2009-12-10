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
#include "fontmanager.h" // bt_global::font
#include "device.h"
#include "device_status.h"
#include "main.h" // bt_global::config
#include "scaleconversion.h"

#include <QLabel>
#include <QHBoxLayout>


BannTemperature::BannTemperature(QWidget *parent, QString where, QString descr, device *dev) : banner(parent)
{
	temperature = -235;
	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());


	QLabel *descr_label = new QLabel;
	descr_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	descr_label->setText(descr);

	temp_label = new QLabel;
	temp_label->setFont(bt_global::font->get(FontManager::SUBTITLE));
	setTemperature();

	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 10, 0);
	l->setSpacing(10);
	l->addWidget(descr_label, 0, Qt::AlignLeft);
	l->addWidget(temp_label, 0, Qt::AlignRight);

	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			SLOT(status_changed(QList<device_status*>)));
}

void BannTemperature::status_changed(QList<device_status*> sl)
{
	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (ds->get_type() == device_status::TEMPERATURE_PROBE)
		{
			stat_var curr_temp(stat_var::TEMPERATURE);
			ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
			temperature = curr_temp.get_val();
			setTemperature();
		}
	}
}

void BannTemperature::setTemperature()
{
	switch (temp_scale)
	{
		case CELSIUS:
			temp_label->setText(celsiusString(temperature));
			break;
		case FAHRENHEIT:
			// we don't have a direct conversion from celsius degrees to farhrenheit degrees
			temp_label->setText(fahrenheitString(bt2Fahrenheit(celsius2Bt(temperature))));
			break;
		default:
			qWarning("BannTemperature: unknown scale");
	}
}
