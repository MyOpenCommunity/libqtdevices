#ifndef _VIDEOCITOF_H_
#define _VIDEOCITOF_H_

#include "device_status.h"

#include <QFrame>
#include <QString>
#include <QList>

class postoExt;
class BtButton;
class device;
class QLabel;

// TOdO: rendere un QWidget!
class call_notifier : public QFrame
{
Q_OBJECT
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
signals:
	//! A frame is available
	void frame_available(char *);
	//! Frame was interesting for us
	void frame_captured(call_notifier *);
	//! Emitted when window is closed
	void closed(call_notifier *);
};


class call_notifier_manager : public QObject
{
Q_OBJECT
private:
	//! Pointer to unknown station's notifier
	call_notifier *unknown_station_notifier;
	//! True when a frame comes from a known station
	bool known_station;
public:
	//! Constructor
	call_notifier_manager();
	//! Add call notifier
	void add_call_notifier(call_notifier *);
	//! Set unknown call notifier
	void set_unknown_call_notifier(call_notifier *);
public slots:
	//! A frame is available, pass it on to call notifiers
	//void frame_available_handler(char *);
	//! Invoked when a frame notifier has captured a frame
	void frame_captured_handler(call_notifier *);
	//! Classical gestFrame
	void gestFrame(char *);
signals:
	//! Emitted when a frame is available
	void frame_available(char *);
	//! Emitted when a frame was captured
	void frame_captured(call_notifier *);
	//! Emitted when a call notifier window is closed
	void call_notifier_closed(call_notifier *);
};


#endif // _VIDEOCITOF_H_
