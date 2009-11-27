#ifndef BANN_AIRCONDITIONING_H
#define BANN_AIRCONDITIONING_H

#include "bann2_buttons.h"


class SingleSplit : public BannOnOffNew
{
Q_OBJECT
public:
	SingleSplit(QWidget *parent);
};


class GeneralSplit : public BannOnOffNew
{
Q_OBJECT
public:
	GeneralSplit(QWidget *parent);
};


class AdvancedSplit : public bann2But
{
Q_OBJECT
public:
	AdvancedSplit(QWidget *parent, QString icon_cmd, QString icon_settings, QString descr);
};


#endif // BANN_AIRCONDITIONING_H
