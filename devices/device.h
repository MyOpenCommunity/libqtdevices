#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "device_status.h"
#include "frame_receiver.h"

#include <QVariant>
#include <QHash>
#include <QList>
#include <QTimer>

class frame_interpreter;
class Client;
class OpenMsg;

typedef QHash<int, QVariant> StatusList;


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
	// TODO: The old init with force parameter, to be removed
	void init(bool force);

	// Init device: send messages to initialize data. Every device should
	// re-implement this method, in order to update the related graphic object
	// with the right value.
	virtual void init() { init(false); }

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
	virtual QString get_key();
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

	void sendCommand(QString what, QString _where) const;
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


/********************* Specific class device children classes **********************/

//! MCI
class mci_device : public device
{
Q_OBJECT
public:
	//! Constructor
	mci_device(QString, bool p=false, int g=-1);
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
	virtual QString get_key();
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

//! Aux device
class aux_device : public device
{
Q_OBJECT
public:
	aux_device(QString w, bool p=false, int g=-1);
	virtual void init();

	virtual void manageFrame(OpenMsg &msg);

private:
	stat_var status;

signals:
	// TODO: rimpiazzare questo segnale con lo status_changed(QHash<>)..
	void status_changed(stat_var);
};


#endif //__DEVICE_H__
