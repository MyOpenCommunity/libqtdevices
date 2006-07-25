
#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include <qdatetime.h>
#include "timescript.h"
#include "bannfrecce.h"
#include "device.h"
#include "frame_interpreter.h"

/*!
  \class scenEvo_cond
  \brief This class represents a generic advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond : public QFrame {
    Q_OBJECT
 private:
    static const int MAX_EVO_COND_IMG =5;
    QString *img[MAX_EVO_COND_IMG];
    int val;
    int serial_number;
 public:
    scenEvo_cond(QWidget *parent, char *name) ;
    /*!
      \brief: Returns image path for a certain index
      \arg: index of image whose path shall be returned
    */
    const char *getImg(int index);
    /*!
      \brief sets image path for a certain index
      \param index index of image whose path shall be set
    */
    void setImg(int index, const char *);
    /*!
      \brief returns value related to condition
    */
    int getVal(void);
    /*!
      \brief sets value related to condition
      \param v value to be set
    */
    void setVal(int v);
    /*!
      \brief returns description of condition
    */
    virtual const char *getDescription() ;
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
    /*!
      \brief Sets the background pixmap for the banner.
      \arg pm path of the pixmap
    */     
    //virtual int 	setBGPixmap(char* pm);
    /*!
      \brief Draws and initializes some connections.
    */ 
    virtual void  mostra();
    //! Sets icons
    virtual void SetIcons();
    //! Enable/disable
    virtual void setEnabled(bool);
    //! Save condition
    virtual void save();
    //! Reset condition (on cancel)
    virtual void reset();
    //! Set serial number
    void set_serial_number(int);
    //! Get serial number
    int get_serial_number();
    //! Manages incoming frame
    //virtual void gestFrame(char *);
    //! Inits condition
    virtual void inizializza(void);
    //! Returns true when condition is satisfied
    virtual bool isTrue(void);
public slots:
     void Next();
     void Prev();
     virtual void OK(); 
     virtual void handle_frame(char *);
signals:
    //! Emitted when user clicks Next icon
    void SwitchToNext();
    //! Emitted when user clicks Prev icon (NO MORE !!!)
    void SwitchToPrev();
    //! Emitted when user ckicks Prev icon
    void SwitchToFirst();
    //! Emitted when condition is true
    void verificata();
    //! Status request
    void richStato(char *);
    //! A frame is available
    void frame_available(char *);
};

/*!
  \class scenEvo_cond_h
  \brief This class represent a time based advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond_h : public scenEvo_cond {
    Q_OBJECT
 private:
    //! OK button index (area #7)
    static const int A7_BUTTON_INDEX = 4;
    //! Area #6 (prev) button index 
    static const int A6_BUTTON_INDEX = 5;
    //! Area #8 (next) button index
    static const int A8_BUTTON_INDEX = 6;
    //! OK icon index (area #7)
    static const int A7_ICON_INDEX = 3;
    //! Area #6 (prev) icon index
    static const int A6_ICON_INDEX = 1;
    //! Area #8 (next) icon index
    static const int A8_ICON_INDEX = 2;
    //! Hours, minutes and seconds */
    QString *h, *m, *s;
    //! Pointer to condition time
    QDateTime* cond_time;
    //! Pointers to buttons
    BtButton*  but[7];
    //! Pointer to label
    BtLabel* Immagine;
    //! Time modification object
    timeScript *ora;
    //! Timer for triggering condition
    QTimer *timer;
 public:
    scenEvo_cond_h(QWidget *parent, char *name);
    /*!
      \brief Sets hours
      \param pointer to hours string
    */
    void set_h(const char *h);
    /*!
      \brief Sets minutes
      \param pointer to minutes string
    */
    void set_m(const char *m);
    /*!
      \brief Sets seconds 
      \param pointer to seconds string
    */
    void set_s(const char *s);
    /*!
      \brief Returns condition description
    */
    const char *getDescription();
    /*!
      \brief Draws page and initializes some connections
    */
    void mostra();
    /*!
      \brief Sets the background pixmap for the banner.
      \arg pm path of the pixmap
    */     
    //int 	setBGPixmap(char* pm);
    /*!
      \brief Sets the background color for the banner.
      \param c QColor description of the color.
    */           
    void 	setBGColor(QColor c);
    /*!
      \brief Sets the foreground color for the banner.
      \param c QColor description of the color.
    */         
    void 	setFGColor(QColor c);
    //! Sets icons
    void SetIcons();
    //! Set enabled/disabled
    void setEnabled(bool);
    //! Save condition
    void save();
    //! Reset condition
    void reset();
    //! Return true when condition is satisfied
    bool isTrue(void);
public slots:
    //! OK method
    void OK();
    //! Timer expired method
    void scaduta();
};

class scenEvo_cond_d ;

/*! 
  \class device_condition
  \brief This class represent a device based condition
  \author Ciminaghi
  \date May 2006
*/
class device_condition : public QObject {
    Q_OBJECT
 private:
    //! Condition value
    int cond_value;
    //! Current value (displayed, not confirmed)
    int current_value;
    //! Pointer to parent scenEvo_cond_d
    QWidget *parent;
    //! Condition serial number
    ///int serial_number;
 protected:
    QFrame *frame;
    // Our "real" device 
    device *dev;
    //! True when condition is satisfied
    bool satisfied ;
 public:
    //! Constructor
    device_condition(QWidget *parent, QString *trigger);    
    //! Returns min value
    virtual int get_min();
    //! Returns max value
    virtual int get_max();
    //! Returns step
    virtual int get_step();
    //! Returns value divisor
    virtual int get_divisor();
    //! Returns true if OFF must be shown instead of 0
    virtual bool show_OFF_on_zero();
    //! Returns condition's serial number
    //int get_serial_number();
    //! Sets condition's serial number
    //void set_serial_number(int);
    //! Returns pointer to parent scenEvo_cond_d
    scenEvo_cond_d *get_parent(void);
    //! Sets condition value
    int set_condition_value(int);
    //! Translates trigger condition from open encoding to int and sets val
    virtual int set_condition_value(QString);
    //! Translates current trigger condition to open
    virtual void get_condition_value(QString&);
    //! Gets condition value
    virtual int get_condition_value(void);
    //! Gets condition's meas unit
    virtual void get_unit(QString&);
    //! Shows condition
    void show();
    //! Sets geometry
    void setGeometry(int, int, int ,int);
    //! Draws frame
    virtual void Draw();
    //! Returns current value for condition
    int get_current_value();
    //! Sets current value for condition
    int set_current_value(int);
    //! Sets foreground color
    void setFGColor(QColor c);
    //! Sets background color
    void setBGColor(QColor c);
    //! Inits condition
    virtual void inizializza(void);
    //! Resets condition
    void reset(void);
    //! Checks condition
    //virtual void gestFrame(char *);
    /*
      \brief Returns true if the object is a target for message
    */
    //virtual bool isForMe(openwebnet& message);
    //! Set device where
    void set_where(QString);
    //! Set device pul
    void set_pul(bool);
    //! Set device group
    void set_group(int);
    //! Set enabled/disabled
    void setEnabled(bool);
    //! Returns true when actual condition is satisfied
    bool isTrue(void);
public slots:
    //! Invoked when UP button is pressed
    virtual void Up();
    //! Invoked when DOWN button is pressed
    virtual void Down();
    //! Invoked when OK button is pressed
    void OK();
    //! Invoked by device when status changes
    virtual void status_changed(QPtrList<device_status>);
    //! Invoked when a frame is available
    void handle_frame(char *s);
signals:
    //! No more emitted when condition is true
    // void verificata();
    //! Status request
    void richStato(char *);
    //! Emitted when a frame is received
    void frame_available(char *);
};

/*!
  \class device_condition_light_status
  \brief This class represents a light status based condition
  \author Ciminaghi
  \date May 2006
*/
class device_condition_light_status : public device_condition
{
    Q_OBJECT 
 private:
    //! Returns string to be displayed as a function of value
    void get_string(QString&);
 public:
    //! Constructor
    device_condition_light_status(QWidget *parent, char *name, 
				  QString *trigger); 
    //! Draws frame
    void Draw();
    //! Returns max value
    int get_max();
    //! Translates trigger condition from open encoding to int and sets val
    int set_condition_value(QString);
    //! Translates current trigger condition to open
    void get_condition_value(QString&);
    //! Decodes incoming frame
    //void gestFrame(char*);
#if 0
    //! Inits condition
    void inizializza();
#endif
public slots:
    //! Invoked when status changes
    void status_changed(QPtrList<device_status>);
};

/*!
  \class device_condition_dimming
  \brief This class represents a dimming value based condition
  \author Ciminaghi
  \date May 2006
*/
class device_condition_dimming : public device_condition {
    Q_OBJECT
 private:
 public:
    //! Constructor
    device_condition_dimming(QWidget *parent, char *name, QString *trigger); 
    //! Returns min value
    int get_min();
    //! Returns max value
    int get_max();
    //! Returns step
    int get_step();
    //! Gets condition's meas unit
    void get_unit(QString&);
    //! Returns true if OFF must be shown instead of 0
    bool show_OFF_on_zero();
#if 0
    //! Draws condition
    void Draw();
#endif
    //! Translates trigger condition from open encoding to int and sets val
    int set_condition_value(QString);
    //! Translates current trigger condition to open
    void get_condition_value(QString&);
    //! Decodes incoming frame
    //void gestFrame(char*);
#if 0
    //! Inits condition
    void inizializza();
#endif
public slots:
    //! Invoked when UP button is pressed
    void Up();
    //! Invoked when DOWN button is pressed
    void Down();
    //! Invoked when status changes
    void status_changed(QPtrList<device_status>);
};

/*!
  \class device_condition_volume
  \brief This class represents a volume based condition
  \author Ciminaghi
  \date May 2006
*/
class device_condition_volume : public device_condition
{
    Q_OBJECT
 private:
 public:
    //! Constructor
    device_condition_volume(QWidget *parent, char *name, QString *trigger); 
#if 1
    //! Returns min value
    int get_min();
    //! Returns max value
    int get_max();
#endif
    //! Returns step
    //int get_step();
    //! Draws condition
    void Draw();
    //! Translates trigger condition from open encoding to int and sets val
    int set_condition_value(QString);
    //! Translates current trigger condition to open
    //void get_condition_value(QString&);
    //! Read current value (%)
    //int get_current_value(void);
    //! Decodes incoming frame
    //void gestFrame(char*);
    //! Gets condition's meas unit
    void get_unit(QString&);
#if 0
    //! Inits condition
    void inizializza();
#endif
public slots:
    //! Invoked when status changes
    void status_changed(QPtrList<device_status>); 
    //! Invoked when UP button is pressed
    void Up();
    //! Invoked when DOWN button is pressed
    void Down(); 
};

/*!
  \class device_condition_temp
  \brief This class represents a temperature based condition
  \author Ciminaghi
  \date May 2006
*/
class device_condition_temp : public device_condition
{
    Q_OBJECT
 private:
 public:
    //! Constructor
    device_condition_temp(QWidget *parent, char *name, QString *trigger); 
    //! Returns min value
    int get_min();
    //! Returns max value
    int get_max();
    //! Returns step
    int get_step();
    //! Returns divisor
    int get_divisor();
    //! Gets condition's meas unit
    void get_unit(QString&);
#if 1
    //! Draws condition
    void Draw();
#endif
    //! Returns value
    int intValue();
    //! Translates trigger condition from open encoding to int and sets val
    int set_condition_value(QString);
    //! Translates current trigger condition to open
    void get_condition_value(QString&);
    //! Decodes incoming frame
    void gestFrame(char*);
#if 0
    //! Inits condition
    void inizializza();
#endif
public slots:
    //! Invoked when status changes
    void status_changed(QPtrList<device_status>);
};

/*!
  \class scenEvo_cond_d
  \brief This class represent a device based advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond_d : public scenEvo_cond {
    Q_OBJECT
 private:
    //! Button width/height
    static const int BUTTON_DIM = 60;
    //! Text area width
    static const int TEXT_X_DIM = 180;
    //! Text area height
    static const int TEXT_Y_DIM = 20;
    //! Condition description
    QString *descr;
    //! Device address (open protocol)
    QString *where;
    //! Trigger condition
    QString *trigger;
    //! UP button index (area #3)
    static const int A3_BUTTON_INDEX = 0;
    //! Down button index (area #4)
    static const int A4_BUTTON_INDEX = 1;
    //! Area #5 (OK) button index 
    static const int A5_BUTTON_INDEX = 2;
    //! Area #6 (prev) button index
    static const int A6_BUTTON_INDEX = 3;
    //! Area #1 icon index (symbol) 
    static const int A1_ICON_INDEX = 0;
    //! Area #3 icon index (up)
    static const int A3_ICON_INDEX = 1;
    //! Area #4 icon index (down)
    static const int A4_ICON_INDEX = 2;
    //! Area #5 (OK) icon index
    static const int A5_ICON_INDEX = 3;
    //! Area #7 (prev) icon index
    static const int A6_ICON_INDEX = 4;
    //! Pointers to buttons
    BtButton*  but[7];
    //! Pointer to area1 label
    BtLabel *area1_ptr;
    //! Pointer to area2 label
    BtLabel *area2_ptr;
    //! Specific device condition
    device_condition *actual_condition;
    //! Set button icons
    void SetButtonIcon(int icon_index, int button_index);
    //! Manages incoming frame
    void gestFrame(char *);
    //! Inits condition
    void inizializza(void);
 public:
    scenEvo_cond_d(QWidget *parent, char *name);
    /*!
      \brief Set description
      \param d new description
    */
    void set_descr(const char *d);
    /*
      \brief get <descr> value
      \param output <descr>
    */
    void get_descr(QString&);
    /*!
      \brief Set device address
      \param w new device address
    */
    void set_where(const char *w);
    /*!
      \brief Read device address
    */
    void get_where(QString&);
    /*!
      \brief get trigger condition
      \param t output 
    */
    void get_trigger(QString&);
    /*!
      \brief Set trigger condition
      \param t new trigger condition
    */
    void set_trigger(const char *t);
    /*!
      \brief Returns condition description in human language
    */
    const char *getDescription(void);
    /*!
      \brief Draws and initializes some connections.
    */ 
    void  mostra();
    /*!
      \brief Sets the background color for the banner.
      \param c QColor description of the color.
    */           
    void 	setBGColor(QColor c);
    /*!
      \brief Sets the foreground color for the banner.
      \param c QColor description of the color.
    */         
    void 	setFGColor(QColor c);
    //! Set enabled/disabled
    void setEnabled(bool);
    //! Sets icons
    void SetIcons();
    //! Save condition
    virtual void save();
    //! Reset condition
    void reset();
    //! Return true when condition is satisfied
    bool isTrue(void);
public slots:
    //! OK method
    void OK();
    //! Up method
    void Up();
    //! Down method
    void Down();
    //! Invoked when actual device condition has been triggered by a frame
    //void device_condition_triggered(void);
signals:
};


#endif // _SCENEVOCOND_H_


