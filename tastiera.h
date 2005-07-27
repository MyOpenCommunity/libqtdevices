/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** tastiera.h
**
**definizioni della pagina di tastiera numerica
**
****************************************************************/

#ifndef TASTIERA_H
#define TASTIERA_H

#include <qframe.h>
#include "btlabel.h"
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include <qcursor.h>
    
/*!
  \class tastiera
  \brief This class is the implementation of a keypad intended for password inserting.

  \author Davide
  \date lug 2005
*/  

class  tastiera : public QWidget
{
   Q_OBJECT
public:
    tastiera( QWidget *parent=0, const char *name=0 );
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
  \brief Sets the background pixmap for the banner.
*/  
   int 	setBGPixmap(char* );
   BtButton *zeroBut, *unoBut,*dueBut,*treBut,*quatBut,*cinBut,*seiBut, *setBut, *ottBut, *novBut, *cancBut, *okBut;
   BtLabel* digitLabel, *scrittaLabel;
/*!
  \brief Draws the page
*/     
   void 	draw(); 
/*!
  \brief Selects the mode of operation (encripted or not).
*/     
   void	setMode(char);
   
/*! \enum    tastiType
  differentiate between encripted and clean mode*/
enum tastiType{
                HIDDEN,	/*!< When the code is composed the only a \a * is shown for each digit on the display */
                CLEAN		/*!< When the code is composed the digits are shown on the display*/
   };    
signals:
/*!
  \brief Emitted when the object is closed. The argument represent the code composed by the user.
*/     
    void 	Closed(char*);
    
public slots:
/*!
  \brief Executed when the \a ok \a button is clicked. Hides the object and closes.
*/       
    void ok();
/*!
  \brief Executed when the \a canc \a button is clicked. Hides the object and closes with NULL result.
*/       
    void canc();
    
    void showTastiera();
/*!
  \brief Executed when the \a zero \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/           
    void press0();
/*!
  \brief Executed when the \a one \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press1();
/*!
  \brief Executed when the \a two \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press2();
/*!
  \brief Executed when the \a three \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press3();
/*!
  \brief Executed when the \a four \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press4();
/*!
  \brief Executed when the \a five \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press5();
/*!
  \brief Executed when the \a six \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press6();
/*!
  \brief Executed when the \a seven \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press7();
/*!
  \brief Executed when the \a eight \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press8();
/*!
  \brief Executed when the \a nine \a button is clicked. Composes the code and give a visual feed-back of the new digit.
*/               
    void press9();    
        
private:
    char 	pwd[6];
    char mode;
};



#define BUT_DIM	60
#define LINE 		MAX_HEIGHT/5
#define POSX1		(MAX_WIDTH-BUT_DIM*3)/6
#define POSX2		POSX1*3+BUT_DIM
#define POSX3		POSX2+POSX1*2+BUT_DIM

#endif // TASTIERA_H
