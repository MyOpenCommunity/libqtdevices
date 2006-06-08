/****************************************************************
**
** Definizione della classe della pagina principale
**
****************************************************************/

#ifndef BT_HOMEPAGE
#define BT_HOMEPAGE

#include "btbutton.h"
#include "timescript.h"
#include "openclient.h"
#include "btlabel.h"

#include <qlcdnumber.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qptrlist.h>

/*!
  \class homePage
  \brief This is the class used to implement the main page from which access to all the subtree (sottoMenu) of the various bticino functions. This class is also used for the special page.
  
  It can be composed of a date field, a time field, up to three temperatures, a number of BtButton and a special command. 
  \author Davide
  \date lug 2005
*/  

class homePage : public QWidget
{
    Q_OBJECT
public:
   homePage( QWidget *parent=0, const char *name=0, WFlags f =Qt::WType_TopLevel | Qt::WStyle_Maximize | Qt::WRepaintNoErase);
   // void showPip();
/*!
  \brief Sets the background color for the banner.
  
  The arguments are RGB components for the color.
*/      
   void 	setBGColor(int , int , int );
/*!
  \brief Sets the foreground color for the banner.
  
  The arguments are RGB components for the color.
*/    
   void 	setFGColor(int , int , int );
/*!
  \brief Adds a button to the page.
  
  The arguments are the position of the button (x,y),  the fileName of the image to put on the button, the function associated to the button as described in pagSecLiv (main.h), the \a Open parameters of the commands of the special command (who, what, where), and the type of the special command as descibed in tipoFunzSpe.
*/      
   void 	addButton(int x=0 , int y=0 , char* iconName=NULL, char function = 0,char* chi=NULL, char* cosa=NULL, char* dove=NULL, char tipo=0);
/*!
  \brief Adds a Clock.
  
  The arguments are the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel.
*/      
   void	addClock(int, int, int, int, QColor, QColor, int, int );
/*!
  \brief Same as above. Dimension is assumed 185x35, the colors used are the same of the entire page, with no frame and line.
*/         
   void 	addClock(int , int );   
/*!
  \brief Adds a Date.
  
  The arguments are the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel.
*/      
   void	addDate(int, int, int, int, QColor, QColor, int, int );   
/*!
  \brief Same as above. Dimension is assumed 185x35, the colors used are the same of the entire page, with no frame and line.
*/     
   void 	addDate(int , int );   
/*!
  \brief Adds a Temperature.
  
  The arguments are the zone associated to the temperature measurement, the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel and the text describing the zone.
*/      
   void	addTemp(char*, int, int, int, int, QColor, QColor, int, int, char* );   
  /*!
  \brief Same as above. Dimension is assumed 185x35, the colors used are the same of the entire page, with no frame, line and text.
*/     
   void 	addTemp(char*, int , int );   
/*!
  \brief Adds a Description usually used for special page.
  
  The arguments are the text, the position of the field (x,y),  the dimensions (w,h), background and foreground color, style and line as for BtLabel.
*/   
   void	addDescr(char*, int, int, int, int, QColor, QColor, int, int );      
 /*!
  \brief Same as above. Dimension is assumed 160x20, the colors used are the same of the entire page, with no frame, line and text.
*/        
   void 	addDescr(char*, int , int );
/*!
  \brief Sets the background pixmap for the banner.
*/     
   int 	setBGPixmap(char* );
   
   /*! \enum tipoFunzSpe
     This enum let decide the behavior of the special button*/
  enum tipoFunzSpe{
                NORMALE=0,	/*!<  Clicking the \a special \a button the device always make the same function.*/
                CICLICO=1,		/*!< Clicking the \a special \a button the device one time make a \a ON command and then an \a OFF command an so on.*/
                PULSANTE=2 	/*!<  Pushing the \a special \a button the device make a \a ON command while Releasing the button a \a OFF command is made.*/
   };
  
private slots:
  void 	mouseReleaseEvent ( QMouseEvent * );  
  void	freezed(bool);
  void 	gestFrame(char*);
  void 	specFunz();
  void 	specFunzPress();
  void 	specFunzRelease();  
  
 signals:
/*! \brief Emitted when the \a automation subtree(sottoMenu) is required.*/  
  void 	Automazione(); 
/*! \brief Emitted when the \a lighting subtree(sottoMenu) is required.*/  
  void 	Illuminazione(); 
/*! \brief Emitted when the \a anti-intrusion subtree(sottoMenu) is required.*/  
  void 	Antiintrusione(); 
/*! \brief Emitted when the \a electrict control subtree(sottoMenu) is required.*/  
  void 	Carichi(); 
/*! \brief Emitted when the \a thermoregulation subtree(sottoMenu) is required.*/  
  void 	Termoregolazione(); 
/*! \brief Emitted when the \a sound \a diffusion subtree(sottoMenu) is required.*/  
  void 	Difson(); 
/*! \brief Emitted when the \a scenarios subtree(sottoMenu) is required.*/  
  void 	Scenari(); 
/*! \brief Emitted when the \a settings subtree(sottoMenu) is required.*/  
  void 	Settings(); 
/*! \brief Emitted when the \a special \a function is required.*/
  void 	Special();
/*! \brief Emitted when the the object is closed.*/
  void	Close();
/*! \brief Emitted when the objects composing the page has to be frozen.*/
  void	freeze(bool);
/*! \brief Emitted to send \a Open \a frame on the system.*/
  void 	sendFrame(char*);
/*! \brief Emitted when the \a schedulation subtree(sottoMenu) is required.*/
  void	Schedulazioni();
/*! \brief Emitted when the \a advanced scenarios management(sottoMenu) is required */
  void ScenariEvoluti();
/*! \brief Emitted when the \a video door phone management(sottoMenu) is required */
  void Videocitofonia();
private:
// QTimer 	 *orologio;
  timeScript* dataOra;
int xClock,yClock,xTemp,yTemp;
  QPtrList<BtButton> elencoButtons;
  bool	freez;
  char 	zonaTermo1[50],zonaTermo2[50],zonaTermo3[50];
  char * zt[3];
  QLCDNumber 	*temperatura[3];
  char specialFrame[50];
  char chi[10];
  char cosa[10];
  char dove[10];
  char tipoSpecial,tempCont;
  BtLabel *descrizione, *descrTemp[3];
};



#define MAX_BUT_SOTMEN 	9
#define DIM_BUT_HOME 		80 
#define DIM_SPECBUT_HOME 	180 
#define H_SCR_TEMP		20

#endif // BT_HOMEPAGE
