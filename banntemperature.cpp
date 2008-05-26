/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief 
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "banntemperature.h"
#include "bannpuls.h"
#include "fontmanager.h"
#include <qlabel.h>

BannTemperature::BannTemperature(QWidget *parent, const char *name, QDomNode n)
	: banner(parent, name)
{
	conf_root = n;
	probe_descr = conf_root.namedItem("descr").toElement().text();
	temperature = "";
	setChi("4");

	QDomNode addr = n.namedItem("where");
	if (!addr.isNull())
		setAddress(addr.toElement().text().ascii());
	else
		qFatal("[TERMO] obj: %s, no where in configuration", probe_descr.ascii());
	temperature = "45";

	temp_label = new QLabel(this);
	temp_label->setGeometry(DESCR_LABEL_WIDTH, 0, BANPULS_BUT_DIM, BANPULS_ICON_DIM_Y);
	descr_label = new QLabel(this);
	descr_label->setGeometry(0, 0, DESCR_LABEL_WIDTH, BANPULS_ICON_DIM_Y);
//rember to set bg and fg colors to temp_label
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
			float icx = curr_temp.get_val();
			QString qtemp = "";
			char tmp[10];
			if (icx >= 1000)
			{
				icx = icx - 1000;
				qtemp = "-";
			}
			icx /= 10;
			sprintf(tmp, "%.1f", icx);
			qtemp += tmp;
			qtemp +="\272C";
			temperature = qtemp;
			update = true;
		}
	}

	if (update)
		Draw();
}

void BannTemperature::Draw()
{
	QFont aFont;
	FontManager::instance()->getFont(font_tastiera_scritta_label, aFont);
	descr_label->setFont(aFont);
	descr_label->setAlignment(AlignCenter);
	descr_label->setText(probe_descr);

	FontManager::instance()->getFont(font_banTermo_tempMis, aFont);
	temp_label->setFont(aFont);
	temp_label->setAlignment(AlignCenter);
	temp_label->setText(temperature);
	banner::Draw();
}

