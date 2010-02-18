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


//! Generic device
class device : public QObject, FrameReceiver
{
friend class TestDevice;
Q_OBJECT

public:
	// Init device: send messages to initialize data. Every device should
	// re-implement this method, in order to update the related graphic object
	// with the right value.
	virtual void init() { }

	//! Returns cache key
	virtual QString get_key();
	virtual ~device() {}

	static void setClients(const QHash<int, QPair<Client*, Client*> > &c);

	virtual void manageFrame(OpenMsg &msg) {}

signals:
	// TODO: Old Status changed, to be removed asap.
	void status_changed(QList<device_status*>);

	/// The status changed signal, used to inform that a dimension of device
	/// has changed. For some devices, more than one dimension can be changes
	/// at same time, so the int is used as an enum to recognize the dimensions.
	/// Note that using an int is a design choice. In this way the signal is
	/// generic (so the connections can be made in a generic way) and the enum
	/// can be specific for a device, avoiding the coupling between abstract
	/// and concrete device class.
	void status_changed(const StatusList &status_list);

	void monitorUp();
	void monitorDown();

public slots:
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

protected:
	// The costructor is protected only to make device abstract.
	// NOTE: the default openserver id should be keep in sync with the define MAIN_OPENSERVER
	device(QString who, QString where, int oid=0);

	//! The system of the device
	QString who;
	//! The address of the device
	QString where;

	int openserver_id;

	virtual void subscribe_monitor(int who);

	void sendCommand(QString what, QString _where) const;
	void sendCommand(QString what) const;
	void sendRequest(QString what) const;

private:
	static QHash<int, QPair<Client*, Client*> > clients;
};

#endif //__DEVICE_H__
