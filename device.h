//!  Header file for device classes

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "device_status.h"
#include "frame_receiver.h"

#include <QDateTime>
#include <QVariant>
#include <QString>
#include <QObject>
#include <QHash>
#include <QList>
#include <QTimer>

class frame_interpreter;
class Client;
class BtTime;
class OpenMsg;

typedef QHash<int, QVariant> StatusList;


enum AddressType
{
	NOT_MINE = 0,
	ENVIRONMENT,
	GLOBAL,
	P2P,
};

enum PullMode
{
	PULL,
	NOT_PULL,
	PULL_UNKNOWN,
};

/**
 * Check if the address found in a frame is valid for the device.
 *
 * Frame address must be environment or general. Addresses must be complete, ie
 * must have a+pf and extension part (if any).
 * \return true if msg_where includes the device, false otherwise.
 */
AddressType checkAddressIsForMe(const QString &msg_where, const QString &dev_where, PullMode mode);


class FrameCompressor : public QObject
{
Q_OBJECT
public:
	// w is 'what'
	FrameCompressor(int timeout, int w = -1);
	/// If frame_open matches regex, start the timer and save the frame for later use
	bool analyzeFrame(const QString &frame_open);

private slots:
	void emitFrame();

private:
	QTimer timer;
	int what;
	QString frame;
signals:
	void compressedFrame(QString);
};


//! Generic device
class device : public QObject, FrameReceiver
{
friend class TestDevice;
Q_OBJECT

public:
	//! Init device: send messages initializing data
	virtual void init(bool force = false);
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
	QString get_key();
	virtual ~device();

	static void setClients(Client *command, Client *request);
	void installFrameCompressor(int timeout, int what = -1);

	virtual void manageFrame(OpenMsg &msg);

signals:
	/// Old Status changed
	void status_changed(QList<device_status*>);

	/// The status changed signal, used to inform that a dimension of device
	/// has changed. For some devices, more than one dimension can be changes
	/// at same time, so the int is used as an enum to recognize the dimensions.
	/// Note that using an int is a design choice. In this way the signal is
	/// generic (so the connections can be made in a generic way) and the enum
	/// can be specific for a device, avoiding the coupling between abstract
	/// and concrete device class.
	void status_changed(const StatusList &status_list);

	//! Invoked after successful initialization
	void initialized(device_status *);
	//! We want a frame to be handled
	void handle_frame(char *, QList<device_status*>);

public slots:
	//! receive a frame
	virtual void frame_rx_handler(char *);
	//! Initialization requested by frame interpreter
	void init_requested_handler(QString msg);

	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

protected:
	// The costructor is protected only to make device abstract.
	device(QString who, QString where, bool p=false, int g=-1);
	//! Interpreter
	frame_interpreter *interpreter;
	//! List of device stats
	QList<device_status*> stat;
	//! Node's who
	QString who;
	//! Node's where
	QString where;

	/// connect the frame interpreter with the device
	void setup_frame_interpreter(frame_interpreter* i);
	// Send a frame using the frame compressor
	void sendCompressedFrame(const QString &frame) const;
	void sendCompressedInit(const QString &frame) const;

	void sendCommand(QString what) const;
	void sendRequest(QString what) const;

private:
	//! Pul status
	bool pul;
	//! Device's group
	int group;
	//! Number of users
	int refcount;
	FrameCompressor *cmd_compressor;
	FrameCompressor *req_compressor;

	static Client *client_comandi;
	static Client *client_richieste;
};


class PullStateManager
{
friend class TestLightingDevice;
public:
	PullStateManager(PullMode m);
	bool moreFrameNeeded(OpenMsg &msg, bool is_environment);
	PullMode getPullMode();

private:
	int status;
	bool status_requested;
	PullMode mode;
};


class PullDevice : public device
{
Q_OBJECT
public:
	virtual void manageFrame(OpenMsg &msg);
	// parse the frame and put the results into the provided StatusList
	virtual void parseFrame(OpenMsg &msg, StatusList *sl) = 0;
	// different devices may need different status requests (eg. Dimmer100)
	virtual void requestPullStatus() = 0;

protected:
	PullDevice(QString who, QString where, PullMode m);

private:
	PullStateManager state;
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
	void setHolidayDateTime(QDate date, BtTime time, int program);

	/**
	 * Function to set weekend mode (giorno festivo) with end date and time, and program to be executed at the end of weekend mode.
	 * \param date The end date of weekend mode.
	 * \param time The end time of weekend mode.
	 * \param program The program to be executed at the end of weekend mode.
	 */
	void setWeekendDateTime(QDate date, BtTime time, int program);
	void setWeekProgram(int program);

	/**
	 * Sends a frame to the thermal regulator to set the temperature in manual mode.
	 * The frame sent is generic (not winter nor summer).
	 * \param temperature The temperature to be set, ranges from 50 to 400, step is 5.
	 */
	void setManualTemp(unsigned temperature);

	/**
	 * Getter methods that return the maximum temperature allowed by the thermal regulator.
	 * \return The maximum temperature allowed by the thermal regulator, in 10th of Celsius degrees.
	 */
	virtual unsigned maximumTemp() const = 0;
	/**
	 * Getter method that return the minimum temperature allowed by the thermal regulator.
	 * \return The minimum temperature allowed by the thermal regulator , in 10th of Celsius degrees.
	 */
	virtual unsigned minimumTemp() const;

	/**
	 * Getter method for thermal regulator type.
	 * \return The type of thermal regulator.
	 */
	virtual thermo_type_t type() const = 0;

	enum what_t
	{
		SUMMER = 0,
		WINTER = 1,
		GENERIC_MODE = 3,
		TEMPERATURE_SET = 14,            // set the temperature in manual operation, this is a dimension
		HOLIDAY_DATE_END = 30,           // set the end date of holiday mode, this is a dimension (grandezza)
		HOLIDAY_TIME_END = 31,           // set the end time of holiday mode, this is a dimension (grandezza)
		MANUAL_TIMED_END = 32,           // set the end time of timed manual mode

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
		GENERIC_MANUAL_TIMED = 312,      // timed manual operation (generic mode)
		GENERIC_WEEKEND = 315,           // command to set weekend mode
		WEEK_PROGRAM = 3100,             // command to set the program to be executed (generic mode)
		                                 // remember to add the program number to this number
		SCENARIO_PROGRAM = 3200,         // command to set the scenario to be executed (generic mode)
		                                 // remember to add the program number to this number
		HOLIDAY_NUM_DAYS = 33000,        // command to set the number of days of holiday mode (generic mode)
		                                 // remember to add the number of days to this number
	};
protected:
	thermal_regulator(QString where, bool p=false, int g=-1);
private:
	/**
	 * Utility function to set end date for both holiday and weekend mode
	 * \param date The end date of the mode.
	 */
	void setHolidayEndDate(QDate date);

	/**
	 * Utility function to set end time for both holiday and weekend mode
	 * \param time The end time of the mode.
	 */
	void setHolidayEndTime(BtTime time);
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
	void setManualTempTimed(int temperature, BtTime time);

	virtual unsigned maximumTemp() const;
	virtual thermo_type_t type() const;
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

	virtual unsigned maximumTemp() const;
	virtual thermo_type_t type() const;
};

/**
 * Controlled temperature probe device.
 */
class temperature_probe_controlled : public device
{
Q_OBJECT
public:
	temperature_probe_controlled(QString, thermo_type_t, bool _fancoil,
		const char *ind_centrale, const char *indirizzo, bool p=false, int g=-1);

	void setManual(unsigned setpoint);
	void setAutomatic();

	/**
	 * Sets fancoil speed, if present. The values for the parameters are defined in the protocol.
	 * \param speed 0 = auto, 1 = min, 2 = med, 3 = max
	 */
	void setFancoilSpeed(int speed);
	void requestFancoilStatus();
private:
	/// This is the address of the device as read from configuration file (ie. if where= #15#14, simple_where = 15)
	QString simple_where;
	bool fancoil;
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

//! Aux device
class aux_device : public device
{
Q_OBJECT
public:
	aux_device(QString w, bool p=false, int g=-1);
	virtual void init(bool force = false);

	virtual void manageFrame(OpenMsg &msg);

private:
	stat_var status;

signals:
	// TODO: rimpiazzare questo segnale con lo status_changed(QHash<>)..
	void status_changed(stat_var);
};


#endif //__DEVICE_H__
