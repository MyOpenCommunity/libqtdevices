#include "bann_airconditioning.h"
#include "skinmanager.h" // bt_global::skin
#include "fontmanager.h" // bt_global::font
#include "airconditioning_device.h"
#include "btbutton.h"

#include <QLabel> // BannerText
#include <QDebug>


SingleSplit::SingleSplit(QString descr, AirConditioningDevice *d) : BannOnOffNew(0)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_single = bt_global::skin->getImage("air_single");
	QString img_forward = bt_global::skin->getImage("forward");
	initBanner(img_off, img_air_single, img_forward, descr);
	dev = d;
	QObject::connect(left_button, SIGNAL(clicked()), dev, SLOT(sendOff()));
}


GeneralSplit::GeneralSplit(QString descr) : BannOnOffNew(0)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_gen = bt_global::skin->getImage("air_general");
	QString img_forward = bt_global::skin->getImage("forward");
	initBanner(img_off, img_air_gen, img_forward, descr);
}


AdvancedSplit::AdvancedSplit(QWidget *parent, QString descr): bann2But(parent)
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

