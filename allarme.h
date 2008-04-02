#ifndef _ALLARME_H_
#define _ALLARME_H_

#include <qframe.h>

class banner;
class BtLabel;

/*!
  \class allarme
  \brief This class represents an alarm condition
  \author Ciminaghi
  \date April 2006
*/
class allarme : public QFrame {
    Q_OBJECT
 public:
    typedef enum {
	TECNICO,
	INTRUSIONE,
	MANOMISSIONE,
	PANIC,
    } altype;
 private:
    static const int ICON_DIM = 80;
    static const int NAV_DIM = 60;
    banner *bnav;
    BtLabel *Immagine;
    BtLabel *descr;
    //BtLabel *dettagli;
    void SetIcons(char*);
    altype type;
 public:
    allarme(QWidget *parent, const QString & name, char *indirizzo, char *IconaDx, 
	    altype t) ;
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
     void show();
 public slots:
    /*! Draw widget */
    void draw();
    /*! window freezed, disable everything */
    void freezed(bool);
 signals:
    void Next();
    void Prev();
    void Delete();
    void Back();
};

#endif // _ALLARME_H_
