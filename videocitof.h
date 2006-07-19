
#ifndef _VIDEOCITOF_H_
#define _VIDEOCITOF_H_

#include "bannfrecce.h"
#include "device.h"
#include "frame_interpreter.h"

class postoExt;

class call_notifier : public QFrame {
    Q_OBJECT
 private:
    postoExt *my_station;
    device *station_dev;
    //! Pointer to area1 label
    BtLabel *area1_ptr;
    //! Pointer to area3 label
    BtLabel *area3_ptr;
    //! Pointer to area5 label
    BtLabel *area5_ptr;
    //! Pointer to area 2 button
    BtButton *area2_but;
    //! Pointer to area 4 button
    BtButton *area4_but;
    //! Pointer to area 6 button
    BtButton *area6_but;
    //! Set icons in frame
    void SetIcons();
    //! Set buttons' icons
    void SetButtonsIcons();
    //! Set single button icon
    void SetButtonIcon(const char *icon_name, BtButton *but);
    //! Some consts
    static const int LABEL_HEIGHT = 20;
    static const int BUTTON_DIM = 60;
    static const int LABEL_WIDTH = 180;
 public:
    //! Constructor
    call_notifier(QWidget *parent, char *name, postoExt *ms);
    //! Get station's where
    void get_where(QString&);
    /*!
      \brief Sets the background color for the banner.
      \param c QColor description of the color.
    */           
    virtual void 	setBGColor(QColor c);
    /*!
      \brief Sets the foreground color for the banner.
      \param c QColor description of the color.
    */         
    virtual void 	setFGColor(QColor c);
    /*!
      \brief Sets the background color for the banner.
      \param r red component for the color.
      \param g green component for the color.
      \param b blue component for the color.
    */        
    void 	setBGColor(int r, int g, int b);
    /*!
      \brief Sets the background color for the banner.
      \param r red component for the color.
      \param g green component for the color.
      \param b blue component for the color.
    */      
    void 	setFGColor(int r, int g, int b);
 public slots:
    //! Invoked by device when status changes
    void status_changed(QPtrList<device_status>);
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
 signals:
    //! A frame is available
    void frame_available(char *);
    //! Frame was interesting for us
    void frame_captured(call_notifier *);
    //! Emitted when window is closed
    void closed(call_notifier *);
};


class call_notifier_manager : public QObject {
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


