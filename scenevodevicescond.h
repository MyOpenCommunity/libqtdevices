#ifndef SCENEVODEVICESCOND_H
#define SCENEVODEVICESCOND_H

#include <QWidget>
#include <QObject>
#include <QList>

#include "main.h" // TemperatureScale
#include "device.h" // StatusList

class NonControlledProbeDevice;
class aux_device;
class LightingDevice;
class DimmerDevice;
class Dimmer100Device;
class sound_device;
class device_status;
class stat_var;
class QString;
class QLabel;


/**
 * This abstract class is a widget that can be used to represent a device
 * condition. Reimplement the updateText method to format properly the condition
 * when its value changes.
 */
class DeviceConditionDisplay : public QWidget
{
Q_OBJECT
public:
	DeviceConditionDisplay(QWidget *parent);

public slots:
	virtual void updateText(int min_condition_value, int max_condition_value) = 0;

protected:
	QLabel *label;
};


/**
 * This class can be used to represent a device condition which can have only
 * the ON or the OFF status.
 */
class DeviceConditionDisplayOnOff : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayOnOff(QWidget *parent) : DeviceConditionDisplay(parent) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/**
 * This class can be used for represent a dimming device condition.
 */
class DeviceConditionDisplayDimming : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayDimming(QWidget *parent) : DeviceConditionDisplay(parent) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/**
 * This class can be used for represent a volume device condition.
 */
class DeviceConditionDisplayVolume : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayVolume(QWidget *parent) : DeviceConditionDisplay(parent) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/**
 * This class can be used for represent a temperature device condition
 * (in celsius or Fahrenheit, depending from the configuration).
 */
class DeviceConditionDisplayTemperature : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayTemperature(QWidget *parent) : DeviceConditionDisplay(parent) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};



class DeviceCondition : public QObject
{
friend class scenEvo_cond_d;
Q_OBJECT
public slots:
	//! Invoked when UP button is pressed
	virtual void Up();
	//! Invoked when DOWN button is pressed
	virtual void Down();
	//! Invoked when OK button is pressed
	virtual void OK();

signals:
	//! Emitted when the condition on device is satisfied
	void condSatisfied();

protected:
	DeviceCondition(DeviceConditionDisplay *cond_display);

	// The method to update the descriptive text of the condition using the
	// the DeviceConditionDisplay widget.
	void updateText(int min_condition_value, int max_condition_value);

	//! Returns min value
	virtual int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	virtual int get_step();
	//! Sets condition value
	void set_condition_value(int);
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
	//! Gets condition value
	virtual int get_condition_value();
	//! Draws frame
	virtual void Draw() = 0;

	//! Returns current value for condition
	int get_current_value();
	//! Sets current value for condition
	int set_current_value(int);
	//! Inits condition
	virtual void inizializza();
	//! Resets condition
	virtual void reset();

	//! Returns true when actual condition is satisfied
	bool isTrue();

	//! True when condition is satisfied
	bool satisfied;

private:
	//! Condition value
	int cond_value;
	//! Current value (displayed, not confirmed)
	int current_value;
	DeviceConditionDisplay *condition_display;
};


class DeviceConditionLight : public DeviceCondition
{
Q_OBJECT
public:
	//! Constructor
	DeviceConditionLight(DeviceConditionDisplay* condition_display, QString trigger, QString where);
	//! Draws frame
	virtual void Draw();
	//! Returns max value
	virtual int get_max();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

protected:
	virtual void inizializza();

private slots:
	void status_changed(const StatusList &sl);

private:
	LightingDevice *dev;
};


class DeviceConditionDimming : public DeviceCondition
{
Q_OBJECT
public:
	//! Constructor
	DeviceConditionDimming(DeviceConditionDisplay* cond_display, QString trigger, QString where);
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	int get_step();
	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);
	QString get_current_value();
	void reset();
	//! Draws condition
	virtual void Draw();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

public slots:
	void OK();
	//! Invoked when UP button is pressed
	void Up();
	//! Invoked when DOWN button is pressed
	void Down();

protected:
	virtual void inizializza();

private slots:
	void status_changed(const StatusList &sl);

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
	DimmerDevice *dev;
};


class DeviceConditionDimming100 : public DeviceCondition
{
Q_OBJECT
public:
	//! Constructor
	DeviceConditionDimming100(DeviceConditionDisplay* cond_display, QString trigger, QString where);
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	int get_step();
	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);
	QString get_current_value();
	void reset();
	//! Draws condition
	virtual void Draw();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

public slots:
	void OK();
	//! Invoked when UP button is pressed
	void Up();
	//! Invoked when DOWN button is pressed
	void Down();

protected:
	virtual void inizializza();

private slots:
	void status_changed(const StatusList &sl);

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
	Dimmer100Device *dev;;
};


class DeviceConditionVolume : public DeviceCondition
{
Q_OBJECT
public:
	//! Constructor
	DeviceConditionVolume(DeviceConditionDisplay* cond_display, QString trigger, QString where);

	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);
	void get_condition_value(QString& out);
	void reset();
	//! Draws condition
	virtual void Draw();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);


public slots:
	void OK();
	//! Invoked when status changes
	void status_changed(QList<device_status*>);
	//! Invoked when UP button is pressed
	void Up();
	//! Invoked when DOWN button is pressed
	void Down();

protected:
	virtual void inizializza();

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
	sound_device *dev;
};


class DeviceConditionTemperature : public DeviceCondition
{
Q_OBJECT
public:
	//! Constructor
	DeviceConditionTemperature(DeviceConditionDisplay* cond_display, QString trigger, QString where, bool external = false);
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	int get_step();

	//! Draws condition
	virtual void Draw();

	//! Returns value
	int intValue();
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

protected:
	virtual void inizializza();

private slots:
	//! Invoked when status changes
	void status_changed(const StatusList &);

private:
	/// Maximum and minimum values for temperature conditions
	int max_temp, min_temp;
	/// Step value for temperature conditions
	int step;
	TemperatureScale temp_scale;
	NonControlledProbeDevice *dev;
};


class DeviceConditionAux : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionAux(DeviceConditionDisplay* cond_display, QString trigger, QString where);
	virtual void Draw();
	virtual int get_max();
	virtual void set_condition_value(QString);

public slots:
	void OK();

protected:
	virtual void inizializza();

private slots:
	// TODO: use a more generic approach!
	void status_changed(stat_var status);

private:
	/// check if the condition is satisfied
	void check_condition(bool emit_signal);
	bool device_initialized;
	int device_value;
	aux_device *dev;
};


#endif // SCENEVODEVICESCOND_H
