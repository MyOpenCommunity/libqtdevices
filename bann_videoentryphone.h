#ifndef BANN_VIDEOENTRYPHONE_H
#define BANN_VIDEOENTRYPHONE_H

#include "bann4_buttons.h"
#include "frame_receiver.h"
#include "bann2_buttons.h"

#include <QWidget>
#include <QString>
#include <QFrame>


/// Forward Declarations
class call_notifier_manager;
class call_notifier;
class device_status;
class device;


/*!
 * \class postoExt
 * \brief This class represents an outdoor station
 * \author Ciminaghi
 * \date apr 2006
 */
class postoExt : public bann4tasLab
{
Q_OBJECT
public:
	postoExt(QWidget *parent, QString d, QString Icona1, QString Icona2, QString Icona3, QString Icona4,
		QString _where, QString light, QString key, QString unknown);
	//! Read where
	void get_where(QString&);
	//! Read description
	QString get_descr();
	//! Get key mode
	bool get_key();
	//! Get light mode
	bool get_light();
	//! Get light icon name
	void get_light_icon(QString&);
	//! Get key icon name
	void get_key_icon(QString&);
	//! Cancel icon name
	void get_close_icon(QString&);
public  slots:
	//! Invoked when right button is pressed
	void stairlight_pressed();
	//! Invoked when right button is released 
	void stairlight_released();
	//! Invoked when left button is clicked
	void open_door_clicked();
	//! Invoked when a frame has been captured by a call_notifier
	void frame_captured_handler(call_notifier *);
	//! Invoked when a call_notifier window is closed
	void call_notifier_closed(call_notifier *);
private:
	QString where;
	QString descr;
	bool light;
	bool key;
	bool unknown;
	QString light_icon;
	QString key_icon;
	QString close_icon;
	static call_notifier_manager *cnm;
	static call_notifier *unknown_notifier;
signals:
	//! Emitted when a frame is available
	void frame_available(char *);
};


class call_notifier : public QFrame
{
Q_OBJECT
public:
	call_notifier(QWidget *parent, postoExt *ms);
	//! Get station's where
	void get_where(QString&);
public slots:
	//! Invoked by device when status changes
	void status_changed(QList<device_status*>);
	//! A frame is available, pass it on to the device
	void frame_available_handler(char *);
	//! Area 2 button pressed
	void stairlight_pressed();
	//! Area 4 button released
	void stairlight_released();
	//! Area 6 button clicked
	void open_door_clicked();
	//! Close window
	void close();
	//! Show full screen, reimplemented to restart 30secs timer
	void showFullScreen();
private:
	postoExt *my_station;
	device *station_dev;
	//! Pointer to area1 label
	QLabel *area1_ptr;
	//! Pointer to area3 label
	QLabel *area3_ptr;
	//! Pointer to area5 label
	QLabel *area5_ptr;
	//! Pointer to area 2 button
	BtButton *area2_but;
	//! Pointer to area 4 button
	BtButton *area4_but;
	//! Pointer to area 6 button
	BtButton *area6_but;
	//! Set icons in frame
	void SetIcons(QString _txt1, QString _txt2, QString _txt3);
	//! Set buttons' icons
	void SetButtonsIcons();
	//! Set single button icon
	void SetButtonIcon(QString icon_name, BtButton *but);
	//! Some consts
	static const int LABEL_HEIGHT = 20;
	static const int BUTTON_DIM = 60;
	static const int LABEL_WIDTH = 180;
	//! 30secs idle timer
	QTimer *myTimer;
signals:
	//! Frame was interesting for us
	void frame_captured(call_notifier *);
	//! Emitted when window is closed
	void closed(call_notifier *);
};


class call_notifier_manager : public QObject, FrameReceiver
{
Q_OBJECT
public:
	//! Constructor
	call_notifier_manager();
	//! Add call notifier
	void add_call_notifier(call_notifier *);
	//! Set unknown call notifier
	void set_unknown_call_notifier(call_notifier *);
	void manageFrame(OpenMsg &msg);

public slots:
	//! A frame is available, pass it on to call notifiers
	//void frame_available_handler(char *);
	//! Invoked when a frame notifier has captured a frame
	void frame_captured_handler(call_notifier *);

signals:
	//! Emitted when a frame is available
	void frame_available(char *);
	//! Emitted when a frame was captured
	void frame_captured(call_notifier *);
	//! Emitted when a call notifier window is closed
	void call_notifier_closed(call_notifier *);

private:
	//! Pointer to unknown station's notifier
	call_notifier *unknown_station_notifier;
	//! True when a frame comes from a known station
	bool known_station;

};



/**
 * The banner for the call exclusion, used to silence the ringtone for an
 * incoming call (videocall or intercom)
 */
class CallExclusion : public BannOnOffState
{
Q_OBJECT
public:
	CallExclusion();

public slots:
	void excludeCallOff();
	void excludeCallOn();

signals:
	void statusChanged(bool on);

private:
	States curr_status;
};

#endif
