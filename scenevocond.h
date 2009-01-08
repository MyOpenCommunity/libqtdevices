#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include "page.h"
#include "device_status.h"
#include "main.h"

#include <QList>

class BtButton;
class timeScript;
class device;
class QDateTime;
class QLabel;
class QFrame;
class device_condition;


/*!
  \class scenEvo_cond
  \brief This class represents a generic advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond : public Page
{
Q_OBJECT
public:
	/*!
	\brief sets image path for a certain index
	\param index index of image whose path shall be set
	*/
	void setImg(int index, QString s);

	//! A type flag, used because RTTI is disabled.
	bool hasTimeCondition;

	scenEvo_cond();
	/*!
	\brief returns value related to condition
	*/
	int getVal();
	/*!
	\brief sets value related to condition
	\param v value to be set
	*/
	void setVal(int v);
	/*!
	\brief returns description of condition
	*/
	virtual const char *getDescription();
	//! Sets icons
	virtual void SetIcons();
	//! Set serial number
	void set_serial_number(int);
	//! Get serial number
	int get_serial_number();
	//! Inits condition
	virtual void inizializza();
	//! Returns true when condition is satisfied
	virtual bool isTrue();

public slots:
	//! Next button pressed
	virtual void Next();
	//! Prev button pressed
	virtual void Prev();
	//! OK button pressed
	virtual void OK();
	//! Apply button pressed
	virtual void Apply();
	//! Save condition
	virtual void save();
	//! Reset condition (on cancel)
	virtual void reset();

protected:
	/*!
	\brief: Returns image path for a certain index
	\arg: index of image whose path shall be returned
	*/
	QString getImg(int index);

private:
	static const int MAX_EVO_COND_IMG =5;
	QString *img[MAX_EVO_COND_IMG];
	int val;
	int serial_number;

signals:
	//! Emitted when user clicks Next icon
	void SwitchToNext();
	//! Emitted when user clicks Prev icon (NO MORE !!!)
	void SwitchToPrev();
	//! Emitted when user ckicks Prev icon
	void SwitchToFirst();
	//! Emitted when condition is true
	void verificata();
	//! Emitted when all conditions in list must be reset
	void resetAll();
	//! Emitted when all conditions in list are OK (must be saved and applied)
	void okAll();
	//! After a status changed, a condition is satisfied
	void condSatisfied();
};

/*!
  \class scenEvo_cond_h
  \brief This class represent a time based advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond_h : public scenEvo_cond
{
Q_OBJECT
private:
	//! OK button index (area #7)
	static const int A7_BUTTON_INDEX = 4;
	//! Area #6 (prev) button index
	static const int A6_BUTTON_INDEX = 5;
	//! Area #8 (next) button index
	static const int A8_BUTTON_INDEX = 6;
	//! OK icon index (area #7)
	static const int A7_ICON_INDEX = 3;
	//! Area #6 (prev) icon index
	static const int A6_ICON_INDEX = 1;
	//! Area #8 (next) icon index
	static const int A8_ICON_INDEX = 2;
	//! Hours, minutes and seconds */
	QString *h, *m, *s;
	//! Pointer to condition time
	QDateTime *cond_time;
	//! Pointers to buttons
	BtButton *but[7];
	//! Pointer to label
	QLabel *Immagine;
	//! Time modification object
	timeScript *ora;
	//! Timer for triggering condition
	QTimer *timer;
public:
	scenEvo_cond_h();
	/*!
	\brief Sets hours
	\param pointer to hours string
	*/
	void set_h(QString h);
	/*!
	\brief Sets minutes
	\param pointer to minutes string
	*/
	void set_m(QString m);
	/*!
	\brief Sets seconds
	\param pointer to seconds string
	*/
	void set_s(QString s);
	/*!
	\brief Returns condition description
	*/
	const char *getDescription();
	/*!
	\brief Draws page and initializes some connections
	*/
	virtual void showPage();
	//! Sets icons
	void SetIcons();
	//! Save condition
	void save();
	//! Return true when condition is satisfied
	bool isTrue();
public slots:
	//! OK method
	void OK();
	//! Apply method
	void Apply();
	//! Reset condition
	void reset();
	//! Timer expired method
	void scaduta();
	//! Just setup qt timer (based on cond_time)
	void setupTimer();
};


/*!
\class scenEvo_cond_d
\brief This class represent a device based advanced scenario condition
\author Ciminaghi
\date April 2006
*/
class scenEvo_cond_d : public scenEvo_cond
{
Q_OBJECT
private:
	//! Button width/height
	static const int BUTTON_DIM = 60;
	//! Text area width
	static const int TEXT_X_DIM = 180;
	//! Text area height
	static const int TEXT_Y_DIM = 20;
	//! Condition description
	QString *descr;
	//! Device address (open protocol)
	QString *where;
	//! Trigger condition
	QString *trigger;
	//! UP button index (area #3)
	static const int A3_BUTTON_INDEX = 0;
	//! Down button index (area #4)
	static const int A4_BUTTON_INDEX = 1;
	//! Area #5 (OK) button index
	static const int A5_BUTTON_INDEX = 2;
	//! Area #6 (prev) button index
	static const int A6_BUTTON_INDEX = 3;
	//! Area #1 icon index (symbol)
	static const int A1_ICON_INDEX = 0;
	//! Area #3 icon index (up)
	static const int A3_ICON_INDEX = 1;
	//! Area #4 icon index (down)
	static const int A4_ICON_INDEX = 2;
	//! Area #5 (OK) icon index
	static const int A5_ICON_INDEX = 3;
	//! Area #7 (prev) icon index
	static const int A6_ICON_INDEX = 4;
	//! Pointers to buttons
	BtButton *but[7];
	//! Pointer to area1 label
	QLabel *area1_ptr;
	//! Pointer to area2 label
	QLabel *area2_ptr;
	//! Specific device condition
	device_condition *actual_condition;
	//! Set button icons
	void SetButtonIcon(int icon_index, int button_index);
	//! Manages incoming frame
	void gestFrame(char *);
	//! Inits condition
	void inizializza();
public:
	scenEvo_cond_d();
	/*!
		\brief Set description
		\param d new description
	*/
	void set_descr(QString d);
	/*
		\brief get <descr> value
		\param output <descr>
	*/
	void get_descr(QString&);
	/*!
		\brief Set device address
		\param w new device address
	*/
	void set_where(QString w);
	/*!
		\brief Read device address
	*/
	void get_where(QString&);
	/*!
	\brief get trigger condition
	\param t output
	*/
	void get_trigger(QString&);
	/*!
	\brief Set trigger condition
	\param t new trigger condition
	*/
	void set_trigger(QString t);
	/*!
	\brief Returns condition description in human language
	*/
	const char *getDescription();
	/*!
	\brief Draws and initializes some connections.
	*/
	virtual void showPage();
	//! Sets icons
	void SetIcons();
	//! Save condition
	virtual void save();
	//! Return true when condition is satisfied
	bool isTrue();
public slots:
	//! OK method
	void OK();
	//!  Apply method
	void Apply();
	//! Up method
	void Up();
	//! Down method
	void Down();
	//! Invoked when actual device condition has been triggered by a frame
	//void device_condition_triggered();
	//! Reset condition
	void reset();
};


/*! 
  \class device_condition
  \brief This class represent a device based condition
  \author Ciminaghi
  \date May 2006
*/
class device_condition : public QObject
{
Q_OBJECT
private:
	//! Condition value
	int cond_value;
	//! Current value (displayed, not confirmed)
	int current_value;
	//! Pointer to parent scenEvo_cond_d
	QWidget *parent;
protected:
	QFrame *frame;
	// Our "real" device
	device *dev;
	//! True when condition is satisfied
	bool satisfied;

public:
	//! Constructor
	device_condition(QWidget *parent, QString *trigger);
	//! Returns min value
	virtual int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	virtual int get_step();
	//! Returns value divisor
	virtual int get_divisor();
	//! Returns true if OFF must be shown instead of 0
	virtual bool show_OFF_on_zero();
	//! Returns pointer to parent scenEvo_cond_d
	scenEvo_cond_d *get_parent();
	//! Sets condition value
	void set_condition_value(int);
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
	//! Gets condition value
	virtual int get_condition_value();
	//! Gets condition's meas unit
	virtual QString get_unit();
	//! Shows condition
	void show();
	//! Sets geometry
	void setGeometry(int, int, int ,int);
	//! Draws frame
	virtual void Draw();
	//! Returns current value for condition
	int get_current_value();
	//! Sets current value for condition
	int set_current_value(int);
	//! Inits condition
	virtual void inizializza();
	//! Resets condition
	virtual void reset();
	//! Setup the device
	virtual void setup_device(QString);
	//! Set device pul
	void set_pul(bool);
	//! Set device group
	void set_group(int);
	//! Returns true when actual condition is satisfied
	bool isTrue();
public slots:
	//! Invoked when UP button is pressed
	virtual void Up();
	//! Invoked when DOWN button is pressed
	virtual void Down();
	//! Invoked when OK button is pressed
	virtual void OK();
	//! Invoked by device when status changes
	virtual void status_changed(QList<device_status*>) = 0;
signals:
	//! No more emitted when condition is true
	// void verificata();
	//! Emitted when the condition on device is satisfied
	void condSatisfied();
};

/*!
\class device_condition_light_status
\brief This class represents a light status based condition
\author Ciminaghi
\date May 2006
*/
class device_condition_light_status : public device_condition
{
	Q_OBJECT
private:
	//! Returns string to be displayed as a function of value
	QString get_string();
public:
	//! Constructor
	device_condition_light_status(QWidget *parent, QString *trigger);
	//! Draws frame
	virtual void Draw();
	//! Returns max value
	virtual int get_max();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
public slots:
	//! Invoked when status changes
	virtual void status_changed(QList<device_status*>);
};

/*!
\class device_condition_dimming
\brief This class represents a dimming value based condition
\author Ciminaghi
\date May 2006
*/
class device_condition_dimming : public device_condition
{
Q_OBJECT
private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
public:
	//! Constructor
	device_condition_dimming(QWidget *parent, QString *trigger);
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	int get_step();
	//! Gets condition's meas unit
	virtual QString get_unit();
	//! Returns true if OFF must be shown instead of 0
	bool show_OFF_on_zero();
	void set_condition_value_min(int);
	void set_condition_value_min(QString);
	int get_condition_value_min();
	void set_condition_value_max(int);
	void set_condition_value_max(QString);
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
	//! Decodes incoming frame
	//void gestFrame(char*);
public slots:
	void OK();
	//! Invoked when UP button is pressed
	void Up();
	//! Invoked when DOWN button is pressed
	void Down();
	//! Invoked when status changes
	virtual void status_changed(QList<device_status*>);
};


/*!
\class device_condition_dimming_100
\brief This class represents a dimming 100 value based condition
\author Ciminaghi/Agresta
\date May 2006
*/
class device_condition_dimming_100 : public device_condition
{
Q_OBJECT
private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
public:
	//! Constructor
	device_condition_dimming_100(QWidget *parent, QString *trigger);
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	int get_step();
	//! Gets condition's meas unit
	virtual QString get_unit();
	//! Returns true if OFF must be shown instead of 0
	bool show_OFF_on_zero();
	void set_condition_value_min(int);
	void set_condition_value_min(QString);
	int get_condition_value_min();
	void set_condition_value_max(int);
	void set_condition_value_max(QString);
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
	//! Invoked when status changes
	virtual void status_changed(QList<device_status*>);
};

/*!
\class device_condition_volume
\brief This class represents a volume based condition
\author Ciminaghi
\date May 2006
*/
class device_condition_volume : public device_condition
{
Q_OBJECT
private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
public:
	//! Constructor
	device_condition_volume(QWidget *parent, QString *trigger);

	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	void set_condition_value_min(int);
	void set_condition_value_min(QString);
	int get_condition_value_min();
	void set_condition_value_max(int);
	void set_condition_value_max(QString);
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
	//! Gets condition's meas unit
	virtual QString get_unit();
public slots:
	void OK();
	//! Invoked when status changes
	virtual void status_changed(QList<device_status*>);
	//! Invoked when UP button is pressed
	void Up();
	//! Invoked when DOWN button is pressed
	void Down();
};


/*!
\class device_condition_temp
\brief This class represents a temperature based condition
\author Ciminaghi
\date May 2006
*/
#define CONDITION_MIX_TEMP 1050
#define CONDITION_MAX_TEMP  500
class device_condition_temp : public device_condition
{
Q_OBJECT
public:
	//! Constructor
	device_condition_temp(QWidget *parent, QString *trigger);
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	int get_step();
	//! Returns divisor
	int get_divisor();
	//! Gets condition's meas unit
	virtual QString get_unit();

	//! Draws condition
	virtual void Draw();

	//! Returns value
	int intValue();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);
	//! Decodes incoming frame
	void gestFrame(char*);
public slots:
	//! Invoked when status changes
	virtual void status_changed(QList<device_status*>);
private:
	/// Maximum and minimum values for temperature conditions
	int max_temp, min_temp;
	/// Step value for temperature conditions
	int step;
	TemperatureScale temp_scale;
};


/*!
\class device_condition_aux
\brief This class represent a condition on a aux device
\author Gianni Valdambrini <aleister@develer.com>
*/
class device_condition_aux : public device_condition
{
Q_OBJECT
public:
	device_condition_aux(QWidget *parent, QString *trigger);
	virtual void Draw();
	virtual int get_max();
	virtual void set_condition_value(QString);

	//! Setup the device
	virtual void setup_device(QString);

public slots:
	void OK();

protected slots:
	virtual void status_changed(QList<device_status*> sl);

private slots:
	// TODO: use a more generic approach!
	void status_changed(stat_var status);
private:
	/// check if the condition is satisfied
	void check_condition(bool emit_signal);
	bool device_initialized;
	int device_value;
};

#endif // _SCENEVOCOND_H_
