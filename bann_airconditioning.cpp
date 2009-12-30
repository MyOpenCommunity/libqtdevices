#include "bann_airconditioning.h"
#include "skinmanager.h" // bt_global::skin
#include "fontmanager.h" // bt_global::font
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

		connect(dev_probe, SIGNAL(status_changed(const StatusList &)),
				SLOT(status_changed(const StatusList &)));
	}

	initBanner(img_off, bt_global::skin->getImage(air_single), img_forward, descr);
	connect(left_button, SIGNAL(clicked()), dev, SLOT(sendOff()));
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


AdvancedSplitScenario::AdvancedSplitScenario(QWidget *parent, QString descr): Bann2Buttons(parent)
{
	QString icon_cmd = bt_global::skin->getImage("split_cmd");
	QString icon_settings = bt_global::skin->getImage("split_settings");
	initBanner(icon_cmd, icon_settings, descr);
}


SplitTemperature::SplitTemperature(int init_temp, int level_max, int level_min, int step) :
	Bann2Buttons(0)
{
	QString icon_plus = bt_global::skin->getImage("plus");
	QString icon_minus = bt_global::skin->getImage("minus");
	initBanner(icon_minus, icon_plus, "---", FontManager::SUBTITLE);

	Q_ASSERT_X(init_temp >= level_min && init_temp <= level_max, "SplitTemperature::SplitTemperature",
		"Initial temperature is outside the given range.");
	current_temp = init_temp;
	max_temp = level_max;
	min_temp = level_min;
	temp_step = step;
	updateText();

	left_button->setAutoRepeat(true);
	connect(left_button, SIGNAL(clicked()), SLOT(decreaseTemp()));
	right_button->setAutoRepeat(true);
	connect(right_button, SIGNAL(clicked()), SLOT(increaseTemp()));
}

void SplitTemperature::setTemperature(int new_temp)
{
	if (new_temp >= min_temp && new_temp <= max_temp)
	{
		current_temp = new_temp;
		updateText();
		emit valueChanged(current_temp);
	}
	else
		qWarning() << "SplitTemperature::setTemperature: provided temp is outside limits, ignoring.";
}

void SplitTemperature::increaseTemp()
{
	int tmp = current_temp + temp_step;
	if (tmp <= max_temp)
	{
		current_temp = tmp;
		updateText();
		emit valueChanged(current_temp);
	}
}

void SplitTemperature::decreaseTemp()
{
	int tmp = current_temp - temp_step;
	if (tmp >= min_temp)
	{
		current_temp = tmp;
		updateText();
		emit valueChanged(current_temp);
	}
}

void SplitTemperature::updateText()
{
	// TODO: what about fahrenheit temperature?
	setCentralText(celsiusString(current_temp));
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
	speeds_descr[1] = tr("LOW");
	speeds_descr[2] = tr("MEDIUM");
	speeds_descr[3] = tr("HIGH");

	foreach (int speed_id, speeds)
		if (speeds_descr.contains(speed_id))
			addState(speed_id, speeds_descr[speed_id]);
		else
			qWarning("The speed id %d doesn't exists", speed_id);

	initBanner(bt_global::skin->getImage("cycle"), current_speed);
}


// The button group guarantees that only one button is pressed at a given time.
// Button ids are chosen so that they can be converted to bool values, ie. are 0 or 1.
SplitSwing::SplitSwing(QString descr, bool init_swing) : Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("swing_off"), bt_global::skin->getImage("swing_on"), descr);

	// left = OFF button, so id is 0
	buttons.addButton(left_button, 0);
	// right = ON button, so id is 1
	buttons.addButton(right_button, 1);
	buttons.setExclusive(true);

	right_button->setCheckable(true);
	left_button->setCheckable(true);
	setSwingOn(init_swing);

	connect(&buttons, SIGNAL(buttonClicked(int)), SLOT(handleButtonClick(int)));
}

void SplitSwing::handleButtonClick(int button_id)
{
	// convert int to bool
	bool swing_on = button_id;
	emit swingOn(swing_on);
}

void SplitSwing::setSwingOn(bool swing_on)
{
	swing_on ? right_button->setChecked(true) : left_button->setChecked(true);
}


SplitScenario::SplitScenario(QString descr, QString cmd, AirConditioningDevice *d) : Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("split_cmd"), QString(), descr);
	command = cmd;
	dev = d;
	connect(left_button, SIGNAL(clicked()), SLOT(sendScenarioCommand()));
}

void SplitScenario::sendScenarioCommand()
{
	dev->sendCommand(command);
}


GeneralSplitScenario::GeneralSplitScenario(QString descr) : Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("split_cmd"), QString(), descr);
	connect(left_button, SIGNAL(clicked()), SLOT(sendScenarioCommand()));
}

void GeneralSplitScenario::appendDevice(QString cmd, AirConditioningInterface *d)
{
	devices_list.append(qMakePair(cmd, d));
}

void GeneralSplitScenario::sendScenarioCommand()
{
	for (int i = 0; i < devices_list.size(); ++i)
		devices_list[i].second->activateScenario(devices_list[i].first);
}
