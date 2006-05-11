
#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include <qdatetime.h>
#include "timescript.h"
#include "bannfrecce.h"

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
public slots:
     void Next();
     void Prev();
     virtual void OK(); 
signals:
    //! Emitted when user clicks Next icon
    void SwitchToNext();
    //! Emitted when user clicks Prev icon
    void SwitchToPrev();
    //! Emitted when condition is true
    void verificata();
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
    static const int A7_ICON_INDEX = 1;
    //! Area #6 (prev) icon index
    static const int A6_ICON_INDEX = 3;
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
public slots:
    //! OK method
    void OK();
    //! Timer expired method
    void scaduta();
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
    //! Trigger condition (?)
    QString *trigger;
    //! UP button index (area #3)
    static const int A3_BUTTON_INDEX = 0;
    //! Down button index (area #4)
    static const int A4_BUTTON_INDEX = 1;
    //! OK button index (area #6)
    static const int A6_BUTTON_INDEX = 2;
    //! Area #5 (prev) button index 
    static const int A5_BUTTON_INDEX = 3;
    //! Area #7 (next) button index
    static const int A7_BUTTON_INDEX = 4;
    //! Area #1 icon index (symbol) 
    static const int A1_ICON_INDEX = 0;
    //! Area #3 icon index (up)
    static const int A3_ICON_INDEX = 1;
    //! Area #4 icon index (down)
    static const int A4_ICON_INDEX = 2;
    //! Area #5 (prev) icon index
    static const int A5_ICON_INDEX = 4;
    //! Area #7 (next) icon index
    static const int A7_ICON_INDEX = 3;
    //! Pointers to buttons
    BtButton*  but[7];
    //! Pointer to area1 label
    BtLabel *area1_ptr;
    //! Pointer to area2 label
    BtLabel *area2_ptr;
    //! Set button icons
    void SetButtonIcon(int icon_index, int button_index);
 public:
    scenEvo_cond_d(QWidget *parent, char *name);
    /*!
      \brief Set description
      \param d new description
    */
    void set_descr(const char *d);
    /*!
      \brief Set device address
      \param w new device address
    */
    void set_where(const char *w);
    /*!
      \brief Set trigger condition
      \param t new trigger condition
    */
    void set_trigger(const char *t);
    /*!
      \brief Returs condition description
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
    //! Sets icons
    void SetIcons();
public slots:
    //! OK method
    void OK();
    //! Up method
    void Up();
    //! Down method
    void Down();
};


#endif // _SCENEVOCOND_H_
