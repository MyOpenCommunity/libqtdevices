#ifndef __POWERAMPLIFIER_DEVICE_H__
#define __POWERAMPLIFIER_DEVICE_H__

#include "device.h"

#include <QMap>


class poweramplifier_device : public device
{
Q_OBJECT
public:
	enum status_key_t
	{
		ON_OFF,
		VOLUME,
		PRESET,
		TREBLE,
		BASS,
		BALANCE,
		LOUD
	};

	poweramplifier_device(QString, bool p=false, int g=-1);

	void init(bool force);
	void turn_on();
	void turn_off();

public slots:
	//! receive a frame
	void frame_rx_handler(char *);

private:
	QMap <status_key_t, stat_var> status;
signals:
	void status_changed(QMap<status_key_t, stat_var>);
};

#endif
