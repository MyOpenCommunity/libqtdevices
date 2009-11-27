#include "bann_airconditioning.h"


SingleSplit::SingleSplit(QWidget *parent) : BannOnOffNew(parent)
{
}


GeneralSplit::GeneralSplit(QWidget *parent) : BannOnOffNew(parent)
{
}


AdvancedSplit::AdvancedSplit(QWidget *parent, QString icon_cmd, QString icon_settings, QString descr): bann2But(parent)
{
	SetIcons(0, icon_cmd);
	SetIcons(1, icon_settings);
	setText(descr);
	Draw();
}

