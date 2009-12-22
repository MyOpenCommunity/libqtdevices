#include "bann_airconditioning.h"
#include "skinmanager.h" // bt_global::skin
#include "fontmanager.h" // bt_global::font
#include "airconditioning_device.h"
#include "btbutton.h"
#include "probe_device.h"
#include "main.h" // bt_global::config
#include "scaleconversion.h"

#include <QLabel> // BannerText
#include <QDebug>


SingleSplit::SingleSplit(QString descr, AirConditioningDevice *d, NonControlledProbeDevice *d_probe) : BannOnOffNew(0)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_forward = bt_global::skin->getImage("forward");

	dev_probe = d_probe;
	dev = d;

	QString air_single = "air_single";
	if (dev_probe)
	{
		setInternalText("---");
		air_single = "air_single_temp";
	}

	initBanner(img_off, bt_global::skin->getImage(air_single), img_forward, descr);
	connect(left_button, SIGNAL(clicked()), dev, SLOT(sendOff()));

	connect(dev_probe, SIGNAL(status_changed(const StatusList &)),
			SLOT(status_changed(const StatusList &)));
}

void SingleSplit::status_changed(const StatusList &status_list)
{
	int temp = status_list[NonControlledProbeDevice::DIM_TEMPERATURE].toInt();
	TemperatureScale scale = static_cast<TemperatureScale>(bt_global::config[TEMPERATURE_SCALE].toInt());

	QString text;
	if (scale == FAHRENHEIT)
		text = fahrenheitString(bt2Fahrenheit(temp));
	else
		text = celsiusString(bt2Celsius(temp));
	setInternalText(text);
}

void SingleSplit::inizializza(bool)
{
	if (dev_probe)
		dev_probe->requestStatus();
}

GeneralSplit::GeneralSplit(QString descr) : BannOnOffNew(0)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_gen = bt_global::skin->getImage("air_general");
	QString img_forward = bt_global::skin->getImage("forward");
	initBanner(img_off, img_air_gen, img_forward, descr);
	QObject::connect(left_button, SIGNAL(clicked()), SIGNAL(sendGeneralOff()));
}


AdvancedSplitScenario::AdvancedSplitScenario(QWidget *parent, QString descr): bann2But(parent)
{
	QString icon_cmd = bt_global::skin->getImage("split_cmd");
	QString icon_settings = bt_global::skin->getImage("split_settings");

	SetIcons(0, icon_cmd);
	SetIcons(1, icon_settings);
	setText(descr);
	Draw();
}


SplitTemperature::SplitTemperature(QWidget *parent): bann2But(parent)
{
	QString icon_plus = bt_global::skin->getImage("plus");
	QString icon_minus = bt_global::skin->getImage("minus");

	SetIcons(0, icon_minus);
	SetIcons(1, icon_plus);
	setText("23.5 C"); // temperatura!
	Draw(); // blah! la draw deve essere prima della setFont altrimenti non funziona nulla!
	BannerText->setFont(bt_global::font->get(FontManager::SUBTITLE));
}


SplitMode::SplitMode(QList<int> modes, int current_mode) : BannStates(0)
{
	modes_descr[0] = tr("AUTO");
	modes_descr[1] = tr("HEATING");
	modes_descr[2] = tr("COOLING");
	modes_descr[3] = tr("DRY");
	modes_descr[4] = tr("FAN");

	foreach (int mode_id, modes)
		if (modes_descr.contains(mode_id))
			addState(mode_id, modes_descr[mode_id]);
		else
			qWarning("The mode id %d doesn't exists", mode_id);

	initBanner(bt_global::skin->getImage("cycle"), current_mode);
}


SplitSpeed::SplitSpeed(QList<int> speeds, int current_speed) : BannStates(0)
{
	speeds_descr[0] = tr("AUTO");
	speeds_descr[1] = tr("HIGH");
	speeds_descr[2] = tr("MEDIUM");
	speeds_descr[3] = tr("LOW");

	foreach (int speed_id, speeds)
		if (speeds_descr.contains(speed_id))
			addState(speed_id, speeds_descr[speed_id]);
		else
			qWarning("The speed id %d doesn't exists", speed_id);

	initBanner(bt_global::skin->getImage("cycle"), current_speed);
}


SplitSwing::SplitSwing(QString descr) : BannLeft(0)
{
	left_button->setOnOff();
	left_button->setImage(bt_global::skin->getImage("swing_off"));
	left_button->setPressedImage(bt_global::skin->getImage("swing_on"));
	text->setText(descr);

	status = false;
	connect(left_button, SIGNAL(clicked()), SLOT(toggleSwing()));
}


void SplitSwing::toggleSwing()
{
	status = !status;
	left_button->setStatus(status);
}


SplitScenario::SplitScenario(QString descr, QString cmd, AirConditioningDevice *d) : BannLeft(0)
{
	initBanner(bt_global::skin->getImage("split_cmd"), descr);
	command = cmd;
	dev = d;
	connect(left_button, SIGNAL(clicked()), SLOT(sendScenarioCommand()));
}

void SplitScenario::sendScenarioCommand()
{
	dev->sendCommand(command);
}


GeneralSplitScenario::GeneralSplitScenario(QString descr) : BannLeft(0)
{
	initBanner(bt_global::skin->getImage("split_cmd"), descr);
	connect(left_button, SIGNAL(clicked()), SLOT(sendScenarioCommand()));
}

void GeneralSplitScenario::appendDevice(QString cmd, AirConditioningDevice *d)
{
	devices_list.append(qMakePair(cmd, d));
}

void GeneralSplitScenario::sendScenarioCommand()
{
	for (int i = 0; i < devices_list.size(); ++i)
		devices_list[i].second->sendCommand(devices_list[i].first);
}

