//! Header file for device classes

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>

//! State variable
class stat_var {
 public:
    typedef enum { 
	   SPEED = 0,
	   LEV ,
	   OLD_LEV,
	   TEMP ,
	   ON_OFF ,
	   HH,
	   MM,
	   SS,
	   TEMPERATURE,
	   AUDIO_LEVEL,
      } type;
 private:
    type t;
    //QString descr;
    //! Value
    int val;
    //! Minimum
    int min;
    //! Maximum
    int max;
    //! Step
    int step;
    //! True when initialized
    bool _initialized;
 public:
    //! Constructor
    stat_var(stat_var::type _t, int val = 0, int min = 0, int max = 0, 
	     int step = 1, bool initialized = false);
    //! Get value
    void get_val(int&);
    //! Get value again
    int get_val(void);
    //! Set value
    void set_val(int&);
    //! Get min value
    void get_min(int&);
    //! Set min value
    void set_min(int&);
    //! Get max value
    void get_max(int&);
    //! Set max value
    void set_max(int&);
    //! Get step
    void get_step(int&);
    //! Get step, alternate form
    int get_step(void);
    //! Set step
    void set_step(int&);
    //! Get initialized status
    bool initialized(void);
    //! Invalidate variable contents
    void invalidate(void);
    //! Destructor
    ~stat_var();
};

//! Device status description
class device_status {
 public:
    typedef enum { 
	   LIGHTS = 0,
	   DIMMER ,
	   DIMMER100 ,
	   NEWTIMED,
	   TEMPERATURE_PROBE, 
	   AMPLIFIER,
      } type;
 private:
    //! Type
    type t;
    //! Variables
    QPtrList<stat_var> vars;
    //! True when initialized
    bool _initialized;
 public:
    //! Constructor
    device_status(device_status::type);
    //! Read type
    device_status::type get_type(void);
    //! Set type
    void set_type(device_status::type& t);
    //! Add variable
    bool add_var(int index, stat_var *);
    //! Read variable
    int read(int index, stat_var& out);
    //! Write variable's value
    int write_val(int index, stat_var& val);
    //! Returns true when status initialized
    bool initialized(void);
    //! Invalidate device status
    void invalidate(void);
    //! Destructor
    ~device_status();
};

class device_status_light : public device_status {
 private:
 public:
    enum {
	ON_OFF_INDEX = 0,
    } ind;
    device_status_light(); //{ add_var(new stat_var(ON_OFF, 0, 1, 1)); } ;
    //~device_status_light();
};

class device_status_dimmer : public device_status {
 private:
 public:
    enum {
	LEV_INDEX = 0,
	OLD_LEV_INDEX,
    } ind;
    device_status_dimmer();
    //~device_status_dimmer();
};

class device_status_dimmer100 : public device_status {
 private:
 public:
    enum {
	LEV_INDEX = 0,
	OLD_LEV_INDEX,
	SPEED_INDEX,
    } ind;
    device_status_dimmer100();
    //~device_status_dimmer100();
};

class device_status_new_timed : public device_status {
 private:
 public:
    enum {
	HH_INDEX = 0,
	MM_INDEX,
	SS_INDEX
    } ind;
    device_status_new_timed();
    //~device_status_temp_new();
};

class device_status_temperature_probe : public device_status {
 public:
    enum {
	TEMPERATURE_INDEX = 0,
    } ind;
    device_status_temperature_probe();
};

class device_status_amplifier : public device_status {
 public:
    enum {
	AUDIO_LEVEL_INDEX  = 0,
    } ind;
    device_status_amplifier();
};

class frame_interpreter;

//! Generic device
class device : public QObject {
    Q_OBJECT
 private:
    //! Node's who
    QString who;
    //! Node's where
    QString where;
    //! Pul status
    bool pul;
    //! Device's group
    int group;
    //! Number of users
    int refcount;
 protected:
    //! Interpreter
    frame_interpreter *interpreter;
    //! List of device stats
    QPtrList<device_status> *stat;
 public:
    //! Constructor
    device(QString who, QString where, bool p=false, int g=-1);
    //! Init device: send messages initializing data 
    void init();
    //! Set frame interpreter
    void set_frame_interpreter(frame_interpreter *fi);
    //! Set where
    void set_where(QString);
    //! Set pul status
    void set_pul(bool);
    //! Set group
    void set_group(int);
    //! Increment reference count
    void get();
    //! Decrement reference count, return reference count after decrement
    int put();
    //! Returns cache key
    QString get_key(void);
    //! Destructor
    virtual ~device();
 signals:
    //! Status changed
    void status_changed(device_status *);
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
    void frame_event_handler(device_status *);
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

//! Temperature probe
class temperature_probe : public device
{
    Q_OBJECT
 public:
    //! Constructor
    temperature_probe(QString, bool p=false, int g=-1);
};

//! Sound device
class sound_device : public device
{
    Q_OBJECT
 public:
    //! Constructor
    sound_device(QString, bool p=false, int g=-1);
};

#endif __DEVICE_H__
