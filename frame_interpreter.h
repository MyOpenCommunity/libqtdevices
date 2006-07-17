//! Frame interpreter classes

#ifndef __FRAME_INTERPRETER_H__
#define __FRAME_INTERPRETER_H__

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>
#include <stdlib.h>
#include "openclient.h"
#include "device.h"

class device_status;
class frame_interpreter;

//! Openwebnet where
class openwebnet_where : public QString
{
 private:
    bool estesa;
 public:
    //! Constructor
    openwebnet_where(char *);
    //! Constructor with string input
    openwebnet_where(QString);
    //! Returns true if where is a point to point one, outputs address
    bool pp(int& addr);
    //! Returns true if where is a general one
    bool gen();
    //! Returns true if where is an amb. one. Outputs amb number
    bool amb(int& a);
    //! Returns true if where is a gro. one. Outputs gro number
    bool gro(int& g);
};


//! Extended openwebnet class, adds some useful methods to base openwebnet
class openwebnet_ext : public openwebnet
{
 public:
    //! Constructor
    openwebnet_ext();
    //! Returns true if message is a general one
    bool gen();
    //! Returns true if message is an ambient one. Outputs amb. number
    bool amb(int& a);
    //! Returns true if message is a group one. Outputs gr. number
    bool gro(int& g);
    //! Returns true if message is a point-to-point one. Outputs address
    bool pp(int& out);
    //! Returns target's extended where
    QString get_where();
    //! Returns true if frame_interpreter is a target for this message
    bool is_target(frame_interpreter *);
    //! As above, but receives who also (i.e. doesn't read it from fr.int)
    bool is_target(frame_interpreter *fi, QString who);
    //! As above, but discards message's where and considers wh instead
    bool is_target(frame_interpreter *fi, QString who, QString wh);
};


//! Generic frame interpreter
class frame_interpreter : public QObject {
    Q_OBJECT
 protected:
    //! Who
    QString who;
    //! Where
    QString where;
    //! Pul status
    bool pul;
    //! Device's group
    int group;
 private:
 protected:
    //! Returns true when frame is ours
    virtual bool is_frame_ours(openwebnet_ext);
    //! Request initialization
    void request_init(device_status *);
    //! List of device_status objects to be passed on to clients
    QPtrList<device_status> evt_list;
 public:
    //! Constructor
    frame_interpreter(QString who, QString where, bool pul, int group);
    //! Returns who
    QString get_who(void);
    //! Returns where
    QString get_where(void);
    //! Returns pul
    bool get_pul(void);
    //! Returns group mask
    int get_group(void);
    //! Returns init message given device status
    virtual void get_init_message(device_status *, QString&);
    //! Set where
    void set_where(QString);
    //! Returns true if this f.i. belongs to group g
    bool belongs_to_group(int g);
    //! Returns relevant ambient
    int get_amb(void);
 signals:
    //! An event was generated by this frame
    void frame_event(QPtrList<device_status>);
    //! Request device init
    void init_requested(QString);
 public slots:
    //! Receive a frame
    virtual void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Lights frame interpreter
class frame_interpreter_lights : public frame_interpreter {
    Q_OBJECT
 private:
    //! Set status, lights
    void set_status(device_status_light *ds, int s);
    //! Analyze a frame for a pl status
    void handle_frame(openwebnet_ext, device_status_light *);
    //! Set status, dimmer
    void set_status(device_status_dimmer *ds, int s);
    //! Analyze a frame for a dimmer status
    void handle_frame(openwebnet_ext, device_status_dimmer *);
    //! Set status, dimmer 100
    void set_status(device_status_dimmer100 *ds, int lev, int speed);
    //! Analyze a frame for a dimmer 100 status
    void handle_frame(openwebnet_ext, device_status_dimmer100 *);
    //! Set status, new timed device
    void set_status(device_status_new_timed *ds, int hh, int mm, int ss, 
		    int on = -1);
    //! Analyze a frame for a ...
    void handle_frame(openwebnet_ext, device_status_new_timed *);
 public:
    //! Constructor
    frame_interpreter_lights(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Autom frame interpreter
class frame_interpreter_autom : public frame_interpreter {
    Q_OBJECT
 private:
    //! Set status 
    void set_status(device_status_autom *ds, int s);
    //! Analyze a frame for a pl status
    void handle_frame(openwebnet_ext, device_status_autom *);
 public:
    //! Constructor
    frame_interpreter_autom(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Temperature probe frame interpreter
class frame_interpreter_temperature_probe : public frame_interpreter {
    Q_OBJECT
 private:
    //! Set status, lights
    void set_status(device_status_temperature_probe *ds, int s);
    //! Analyze a frame for a pl status
    void handle_frame(openwebnet_ext, device_status_temperature_probe *);
 public:
    //! Constructor
    frame_interpreter_temperature_probe(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Sound device frame interpreter
class frame_interpreter_sound_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Set status, amplifier
    void set_status(device_status_amplifier *ds, int s, bool on);
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_amplifier *);
 public:
    //! Constructor
    frame_interpreter_sound_device(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Radio frame interpreter
class frame_interpreter_radio_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_radio *);
 public:
    //! Constructor
    frame_interpreter_radio_device(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
    //! Returns true if frame is ours
    bool is_frame_ours(openwebnet_ext);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Doorphone device frame interpreter
class frame_interpreter_doorphone_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Set status, doorphone
    void set_status(device_status_doorphone *ds, int s);
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_doorphone *);
 public:
    //! Constructor
    frame_interpreter_doorphone_device(QString, bool, int);
    //! Returns true when frame is ours (reimplemented for doorphone dev.)
    bool is_frame_ours(openwebnet_ext m);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Anti-intrusion system device frame interpreter
class frame_interpreter_impanti_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_impanti *);
 public:
    //! Constructor
    frame_interpreter_impanti_device(QString, bool, int);
    //! Returns true when frame is ours (reimplemented for impanti dev.)
    bool is_frame_ours(openwebnet_ext m);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Anti-intrusion system device frame interpreter (single area)
class frame_interpreter_zonanti_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_zonanti *);
 public:
    //! Constructor
    frame_interpreter_zonanti_device(QString, bool, int);
    //! Returns true when frame is ours (reimplemented for zonanti dev.)
    bool is_frame_ours(openwebnet_ext m);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Thermal regulator device frame interpreter
class frame_interpreter_thermr_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_thermr *);
    //! As above, but for temperature status
    void handle_frame(openwebnet_ext, device_status_temperature_probe *);
 protected:
    //! Returns true when frame is ours (reimplemented for thermr, device
    bool is_frame_ours(openwebnet_ext);
 public:
    //! Constructor
    frame_interpreter_thermr_device(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

//! Modscen device frame interpreter
class frame_interpreter_modscen_device : public frame_interpreter {
    Q_OBJECT
 private:
    //! Analyze a frame
    void handle_frame(openwebnet_ext, device_status_modscen *);
 protected:
    //! Returns true when frame is ours (reimplemented for modscen device)
    //bool is_frame_ours(openwebnet_ext);
 public:
    //! Constructor
    frame_interpreter_modscen_device(QString, bool, int);
    //! Returns init message given device status
    void get_init_message(device_status *, QString&);
 public slots:
    //! Receive a frame
    void handle_frame_handler(char *, QPtrList<device_status> *);
};

#endif //__FRAME_INTERPRETER_H__
