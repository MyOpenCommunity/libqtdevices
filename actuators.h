/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** actuators.h
 **
 **definizioni dei vari attuatori implementati
 **
 ****************************************************************/

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "bann2_buttons.h" // BannOnOffState
#include "bann1_button.h" // BannSinglePuls
#include "device.h" //StatusList

class LightingDevice;


enum ActuatorType
{
	AUTOMAZ,  /*!< driving as a pulse an automation actuator */
	VCT_SERR, /*!< driving as a pulse a video-doorentrysystem actuator configured as "lock" */
	VCT_LS,   /*!< driving as a pulse a video-doorentrysystem actuator configured as "stairlight" */
};


class SingleActuator : public BannOnOffState
{
Q_OBJECT
public:
	SingleActuator(const QString &descr, const QString &where);
	virtual void inizializza(bool forza = false);

private slots:
	void activate();
	void deactivate();
	void status_changed(const StatusList &status_list);

private:
	LightingDevice *dev;
};


class ButtonActuator : public BannSinglePuls
{
Q_OBJECT
public:
	ButtonActuator(const QString &descr, const QString &_where, int t);

private slots:
	void activate();
	void deactivate();

private:
	int type;
	QString where;
};

#endif
