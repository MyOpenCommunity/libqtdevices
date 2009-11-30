#include "bann_airconditioning.h"
#include "skinmanager.h" // bt_global::skin


SingleSplit::SingleSplit(QWidget *parent, QString descr) : BannOnOffNew(parent)
{
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_single = bt_global::skin->getImage("air_single");
	QString img_forward = bt_global::skin->getImage("forward");
	initBanner(img_off, img_air_single, img_forward, descr);
}


GeneralSplit::GeneralSplit(QWidget *parent, QString descr) : BannOnOffNew(parent)
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


TemperatureSplit::TemperatureSplit(QWidget *parent): bann2But(parent)
{
	QString icon_plus = bt_global::skin->getImage("plus");
	QString icon_minus = bt_global::skin->getImage("minus");

	SetIcons(0, icon_minus);
	SetIcons(1, icon_plus);
	setText("23.5 C"); // temperatura!
	Draw();
}

