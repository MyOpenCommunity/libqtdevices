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
#include "device_status.h"
#include "main.h"
#include "scaleconversion.h"

#include <QLabel>
#include <QFont>


BannTemperature::BannTemperature(QWidget *parent, QString where, QString descr, device *dev) : banner(parent)
{
	probe_descr = descr;
	temperature = -235;
	temp_scale = readTemperatureScale();

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
	QFont aFont;
	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	descr_label->setText(probe_descr);

	FontManager::instance()->getFont(font_banTermo_tempImp, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(Qt::AlignCenter);
	switch (temp_scale)
	{
		case CELSIUS:
			temp_label->setText(celsiusString(temperature));
			break;
		case FAHRENHEIT:
			temp_label->setText(fahrenheitString(temperature));
			break;
		default:
			qWarning("BannTemperature: unknown scale");
	}
	banner::Draw();
}
