/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** impostaTime.h
**
**definizioni della pagine di impostazione data/ora
**
****************************************************************/

#ifndef IMPOSTA_TIME_H
#define IMPOSTA_TIME_H

#include <qframe.h>
//#include <qlabel.h>
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include "timescript.h"
#include "btlabel.h"
//#include <qobject.h>

/*!
  \class impostaTime
  \brief This is the class used to set time and date.
  
  \author Davide
  \date lug 2005
*/  
class  impostaTime : public QFrame
{
   Q_OBJECT
public:
    impostaTime( QWidget *parent=0, const char *name=0 );
/*!
  \brief Sets the background color for the banner.
  
  The argument is the QColor description of the color.
*/           
   void 	setBGColor(QColor);
/*!
  \brief Sets the foreground color for the banner.
  
  The argument is the QColor description of the color.
*/            
   void 	setFGColor(QColor);
 /*!
  \brief Sets the background color for the banner.
  
  The arguments are RGB components for the color.
*/         
   void 	setBGColor(int, int , int );
 /*!
  \brief Sets the foreground color for the banner.
  
  The arguments are RGB components for the color.
*/      
   void 	setFGColor(int , int , int );
 /*!
  \brief Sets the background pixmap for the banner.
*/         
   int 	setBGPixmap(char* );
   BtButton*  but[7];
    
signals:
 /*!
  \brief Emitted when the object is closed.
*/         
    void 	Closed();
//  void 	gestFrame(char*);
 /*!
  \brief Emitted to send \a Open \a frame on the system.
*/             
    void 	sendFrame(char*);

public slots:
/*!
  \brief Draws the object.
*/    
    void 	mostra();
private slots:
    void	OKTime();
    void    OKDate();    
private:
   timeScript* dataOra;
   BtLabel* Immagine;
   //   QProcess* dateChange;
};


#endif // IMPOSTA_TIME_H
