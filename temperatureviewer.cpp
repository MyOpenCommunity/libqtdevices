#include "temperatureviewer.h"
#include "fontmanager.h" // bt_global::font
#include "scaleconversion.h"
#include "page.h"
#include "main.h" // bt_global::config

#include <openmsg.h>

#include <QLCDNumber>
#include <QString>
#include <QLabel>
#include <QDebug>

#define H_SCR_TEMP 20


TemperatureViewer::TemperatureViewer(Page *page) : linked_page(page)
{
	temp_scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());
	subscribe_monitor(4);
}

void TemperatureViewer::add(QString where, int x, int y, int width, int height, QString descr, QString ext)
{
	TemperatureData temp;
	QLCDNumber *l = new QLCDNumber(linked_page);
	int style = QFrame::Plain;
	int line = 3; // line width
	temp.lcd = l;
	temp.lcd->setGeometry(x, y, width, height - H_SCR_TEMP);
	temp.lcd->setFrameStyle(style);
	temp.lcd->setLineWidth(line);
	temp.lcd->setNumDigits(9);
	temp.lcd->setSegmentStyle(QLCDNumber::Flat);

	if (!descr.isEmpty())
	{
		temp.text = new QLabel(linked_page);
		temp.text->setFont(bt_global::font->get(FontManager::TEXT));
		temp.text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		temp.text->setText(descr);
		temp.text->setGeometry(x, y + height - H_SCR_TEMP, width, H_SCR_TEMP);
	}

	temp.ext = ext;
	temp.where = where;
	unsigned default_bt_temp = 1235;
	updateDisplay(default_bt_temp, &temp);
	temp_list.append(temp);
}

void TemperatureViewer::inizializza()
{
	foreach (const TemperatureData &temp, temp_list)
	{
		QString frame = "*#4*" + temp.where;
		if (temp.ext == "0")
			frame += "*0##";
		else
			frame += "00*15#" + temp.where + "##";
		linked_page->sendInit(frame);
	}
}

void TemperatureViewer::updateDisplay(unsigned new_bt_temperature, TemperatureData *temp)
{
	QString displayed_temp;

	switch (temp_scale)
	{
	case CELSIUS:
		displayed_temp = celsiusString(bt2Celsius(new_bt_temperature));
		break;
	case FAHRENHEIT:
		displayed_temp = fahrenheitString(bt2Fahrenheit(new_bt_temperature));
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		temp_scale = CELSIUS;
		displayed_temp = celsiusString(bt2Celsius(new_bt_temperature));
	}
	// qlcdnumber can display the degree sign, but not as utf-8 text.
	// We have to change the char TEMP_DEGREES with the single quote char (')
	displayed_temp = displayed_temp.replace(TEMP_DEGREES, " '");

	temp->lcd->display(displayed_temp);
}

void TemperatureViewer::manageFrame(OpenMsg &msg)
{
	char dove[30];
	strcpy(dove, msg.Extract_dove());
	if (dove[0] == '#')
		strcpy(&dove[0], &dove[1]);

	foreach (TemperatureData temp, temp_list)
	{
		int icx = -1;
		if (temp.ext == "0" && temp.where == QString(dove) && !strcmp(msg.Extract_grandezza(), "0"))
			icx = atoi(msg.Extract_valori(0));
		else if (temp.ext == "1" && !strcmp(msg.Extract_grandezza(),"15") && temp.where.at(0) == dove[0])
			icx = atoi(msg.Extract_valori(1));
		if (icx > -1)
			updateDisplay(icx, &temp);
	}
}


