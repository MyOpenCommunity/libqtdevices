#ifndef SCENEVODEVICESCOND_H
#define SCENEVODEVICESCOND_H

#include <QWidget>
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
class BtButton;

/**
 * This abstract class is an object that can be used to represent a device
 * condition. Inherit and reimplement the updateText method to format properly
 * the condition when its value changes.
 */
class DeviceConditionDisplayInterface
{
public:
	virtual void updateText(int min_condition_value, int max_condition_value) = 0;
};



/**
 * This widget is an abstract class that can be used to display a device condition
 * in a standard way (different depending on the underlying hardware). Reimplement
 * the updateText method to format properly the condition when its value changes.
 */
class DeviceConditionDisplay : public QWidget, public DeviceConditionDisplayInterface
{
Q_OBJECT
public:
	DeviceConditionDisplay(QWidget *parent, QString descr, QString top_icon);

signals:
	void upClicked();
	void downClicked();

protected:
	QLabel *condition;
	BtButton *up_button, *down_button;
};


/**
 * This class can be used to represent a device condition which can have only
 * the ON or the OFF status.
 */
class DeviceConditionDisplayOnOff : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayOnOff(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

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
	DeviceConditionDisplayDimming(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

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
	DeviceConditionDisplayVolume(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

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
	DeviceConditionDisplayTemperature(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};



class DeviceCondition : public QObject
{
Q_OBJECT
public:
	//! Returns true when actual condition is satisfied
	bool isTrue();
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

public slots:
	//! Invoked when UP button is pressed
	virtual void Up();
	//! Invoked when DOWN button is pressed
	virtual void Down();
	//! Invoked when OK button is pressed
	virtual void OK();
	//! Resets condition
	virtual void reset();
	//! Inits condition
	virtual void inizializza();

signals:
	//! Emitted when the condition on device is satisfied
	void condSatisfied();

protected:
	DeviceCondition(DeviceConditionDisplayInterface *cond_display);

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
	//! Gets condition value
	virtual int get_condition_value();
	//! Draws frame
	virtual void Draw() = 0;

	//! Returns current value for condition
	int get_current_value();
	//! Sets current value for condition
	int set_current_value(int);

	//! True when condition is satisfied
	bool satisfied;

private:
	//! Condition value
	int cond_value;
	//! Current value (displayed, not confirmed)
	int current_value;
	DeviceConditionDisplayInterface *condition_display;
};


class DeviceConditionLight : public DeviceCondition
{
Q_OBJECT
public:
	//! Constructor
	DeviceConditionLight(DeviceConditionDisplayInterface* condition_display, QString trigger, QString where);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
	virtual void inizializza();

protected:
	virtual void Draw();
	virtual int get_max();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

private slots:
	void status_changed(const StatusList &sl);

private:
	LightingDevice *dev;
};


class DeviceConditionDimming : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionDimming(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
	virtual void inizializza();

public slots:
	virtual void OK();
	virtual void Up();
	virtual void Down();
	virtual void reset();

protected:
	virtual int get_min();
	virtual int get_max();
	virtual int get_step();
	virtual void Draw();

	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);
	QString get_current_value();

	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

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
	DeviceConditionDimming100(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
	virtual void inizializza();

public slots:
	virtual void OK();
	virtual void Up();
	virtual void Down();
	virtual void reset();

protected:
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

	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	virtual void Draw();

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
	DeviceConditionVolume(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where);
	void get_condition_value(QString& out);
	virtual void inizializza();

public slots:
	virtual void OK();
	virtual void Up();
	virtual void Down();
	virtual void reset();

protected:
	int get_min();
	virtual int get_max();
	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);

	virtual void Draw();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

private slots:
	//! Invoked when status changes
	void status_changed(QList<device_status*>);

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
	DeviceConditionTemperature(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where, bool external = false);
	virtual void get_condition_value(QString&);
	virtual void inizializza();

protected:
	int get_min();
	virtual int get_max();
	int get_step();
	int intValue();
	virtual void Draw();

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
	DeviceConditionAux(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where);
	virtual void inizializza();

public slots:
	void OK();

protected:
	virtual void Draw();
	virtual int get_max();
	virtual void set_condition_value(QString);


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
