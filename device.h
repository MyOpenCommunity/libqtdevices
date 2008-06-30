//! Header file for device classes

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "device_status.h"

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>
#include <qdatetime.h>

class frame_interpreter;
class Client;

//! Generic device
class device : public QObject {
Q_OBJECT

public:
	//! Constructor
	device(QString who, QString where, bool p=false, int g=-1);
	//! Init device: send messages initializing data
	void init(bool force = false);
	//! Set frame interpreter
	void set_frame_interpreter(frame_interpreter *fi);
	//! Set where
	void set_where(QString);
	//! Set pul status
	void set_pul(bool);
	//! Set group
	void set_group(int);
	//! Add device status
	void add_device_status(device_status *);
	//! Increment reference count
	void get();
	//! Decrement reference count, return reference count after decrement
	int put();
	//! Returns cache key
	QString get_key(void);

	void setClients(Client *comandi, Client *monitor, Client *richieste);

	//! Destructor
	virtual ~device();

	void sendFrame(const char *frame);
	void sendInit(const char *frame);

signals:
	//! Status changed
	void status_changed(QPtrList<device_status>);
	//! Invoked after successful initialization
	void initialized(device_status *);
	//! We want to send a frame
	void send_frame(char *);
	//! We want a frame to be handled
	void handle_frame(char *, QPtrList<device_status> *);
public slots:
	//! receive a frame
	void frame_rx_handler(char *);
	//! Deal with frame event
	void frame_event_handler(QPtrList<device_status>);
	//! Initialization requested by frame interpreter
	void init_requested_handler(QString msg);

protected:
	//! Interpreter
	frame_interpreter *interpreter;
	//! List of device stats
	QPtrList<device_status> *stat;

	//! Node's who
	QString who;
	//! Node's where
	QString where;

private:
	//! Pul status
	bool pul;
	//! Device's group
	int group;
	//! Number of users
	int refcount;

	Client *client_comandi;
	Client *client_monitor;
	Client *client_richieste;
};

/********************* Specific class device children classes **********************/

//! MCI
class mci_device : public device
{
Q_OBJECT
public:
	//! Constructor
	mci_device(QString, bool p=false, int g=-1);
};

//! Light (might be a simple light, a dimmer or a dimmer 100)
class light : public device
{
Q_OBJECT
public:
	//! Constructor
	light(QString, bool p=false, int g=-1);
	//! Destructor
	//virtual ~light();
};

//! Dimmer
class dimm : public device
{
Q_OBJECT
public:
	//! Constructor
	dimm(QString, bool p=false, int g=-1);
};

//! Dimmer
class dimm100 : public device
{
Q_OBJECT
public:
	//! Constructor
	dimm100(QString, bool p=false, int g=-1);
};

//! Automatismo
class autom : public device
{
	Q_OBJECT
	public:
		//! Constructor
		autom(QString, bool p=false, int g=-1);
};

class thermal_regulator : public device
{
Q_OBJECT
public:
	void setOff();
	void setSummer();
	void setWinter();
	void setProtection();
	void setHolidayDateTime(QDate date, QTime time, int program);
	void setWeekProgram(int program);
	void requestStatus();

	/**
	 * Sends a frame to the thermal regulator to set the temperature in manual mode.
	 * The frame sent is generic (not winter nor summer).
	 * \param temperature The temperature to be set, ranges from 50 to 400, step is 5.
	 */
	void setManualTemp(int temperature);

	enum
	{
		SUMMER = 0,
		WINTER = 1,
		GENERIC_MODE = 3,
		TEMPERATURE_SET = 14,            // set the temperature in manual operation, this is a dimension
		HOLIDAY_DATE_END = 30,           // set the end date of holiday mode, this is a dimension (grandezza)
		HOLIDAY_TIME_END = 31,           // set the end time of holiday mode, this is a dimension (grandezza)

		// summer specific identifiers
		SUM_PROTECTION = 202,            // protection
		SUM_OFF = 203,                   // off
		SUM_MANUAL = 210,                // manual operation (all zones in setpoint temperature)
		SUM_MANUAL_TIMED = 212,          // manual operation (24h maximum)
		SUM_WEEKEND = 215,               // weekend operation (festivo)
		SUM_PROGRAM = 2100,              // weekly program (1 out of 3)
		SUM_SCENARIO = 2200,             // scenario (1 out of 16, 99zones thermal regulator only)
		SUM_HOLIDAY = 23000,             // holiday operation (programma ferie)

		// winter specific identifiers
		WIN_PROTECTION = 102,
		WIN_OFF = 103,                   // off
		WIN_MANUAL = 110,                // manual operation (all zones in setpoint temperature)
		WIN_MANUAL_TIMED = 112,          // manual operation (24h maximum)
		WIN_WEEKEND = 115,               // weekend operation (festivo)
		WIN_PROGRAM = 1100,              // weekly program (1 out of 3)
		WIN_SCENARIO = 1200,             // scenario (1 out of 16, 99zones thermal regulator only)
		WIN_HOLIDAY = 13000,             // holiday operation (programma ferie)

		// generic identifiers (useful for issuing commands)
		GENERIC_PROTECTION = 302,
		GENERIC_OFF = 303,
		WEEK_PROGRAM = 3100,             // command to set the program to be executed (generic mode)
		                                 // remember to add the program number to this number
		SCENARIO_PROGRAM = 3200,         // command to set the scenario to be executed (generic mode)
		                                 // remember to add the program number to this number
		HOLIDAY_NUM_DAYS = 33000,        // command to set the number of days of holiday mode (generic mode)
		                                 // remember to add the number of days to this number
	};
protected:
	thermal_regulator(QString where, bool p=false, int g=-1);
};

class thermal_regulator_4z : public thermal_regulator
{
Q_OBJECT
public:
	thermal_regulator_4z(QString where, bool p=false, int g=-1);

	/**
	 * Sets the temperature for a limited time.
	 * \param temperature The temperature to be set
	 * \param time The duration of the manual setting (24 hours max?)
	 */
	void setManualTempTimed(int temperature, QTime time);
};

class thermal_regulator_99z : public thermal_regulator
{
Q_OBJECT
public:
	thermal_regulator_99z(QString where, bool p=false, int g=-1);

	/**
	 * Sets the scenario on the thermal regulator.
	 * \param scenario The scenario to be activated (16 max).
	 */
	void setScenario(int scenario);
};

/**
 * Controlled temperature probe device.
 */
class temperature_probe_controlled : public device
{
Q_OBJECT
public:
	//! Constructor
	temperature_probe_controlled(QString, thermo_type_t, bool fancoil,
		const char *ind_centrale, const char *indirizzo, bool p=false, int g=-1);
};

/**
 * Not controlled temperature probe device (external or internal).
 */
class temperature_probe_notcontrolled : public device
{
Q_OBJECT
public:
	//! Constructor
	temperature_probe_notcontrolled(QString, bool external, bool p=false, int g=-1);
};

//! Sound device (ampli)
class sound_device : public device
{
Q_OBJECT
public:
	//! Constructor
	sound_device(QString, bool p=false, int g=-1);
};

//! Radio
class radio_device : public device
{
Q_OBJECT
public:
	//! Constructor
	radio_device(QString, bool p=false, int g=-1);
};

//! Sound matrix
class sound_matr : public device
{
Q_OBJECT
public:
	//! Constructor
	sound_matr(QString, bool p=false, int g=-1);
};

//! Doorphone device
class doorphone_device : public device
{
Q_OBJECT
public:
	//! Constructor
	doorphone_device(QString, bool p=false, int g=-1);
};

//! ??
class impanti_device : public device
{
Q_OBJECT
public:
	//! Constructor
	impanti_device(QString, bool p=false, int g=-1);
};

//! ??
class zonanti_device : public device
{
Q_OBJECT
public:
	//! Constructor
	zonanti_device(QString, bool p=false, int g=-1);
};

//! Modscen device
class modscen_device : public device
{
Q_OBJECT
public:
	//! Constructor
	modscen_device(QString, bool p=false, int g=-1);
};

class poweramplifier_device : public device
{
Q_OBJECT
public:
	poweramplifier_device(QString, bool p=false, int g=-1);
};

#endif //__DEVICE_H__
