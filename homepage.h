/****************************************************************
**
** Definizione della classe della pagina principale
**
****************************************************************/

#ifndef BT_HOMEPAGE
#define BT_HOMEPAGE

#include "btbutton.h"
#include "timescript.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qptrlist.h>


class homePage : public QWidget
{
    Q_OBJECT
public:
   homePage( QWidget *parent=0, const char *name=0, WFlags f =Qt::WType_TopLevel | Qt::WStyle_Maximize | Qt::WRepaintNoErase);
   // void showPip();
   void 	setBGColor(int , int , int );
   void 	setFGColor(int , int , int );
   void 	addButton(int , int , char* , char);
   void 	addClock(int , int );
   void	homePage::addClock(int, int, int, int, QColor, QColor, int, int );
   int 	setBGPixmap(char* );
  
private slots:
  //  void aggOra();
  //  void scriviOra();
  //  void cambiaSchermo(int); 
//  void 	mousePressEvent ( QMouseEvent * );
  void 	mouseReleaseEvent ( QMouseEvent * );  
  void	freezed(bool);
  
 signals:
  void 	Automazione(); 
  void 	Illuminazione(); 
  void 	Antiintrusione(); 
  void 	Carichi(); 
  void 	Termoregolazione(); 
  void 	Difson(); 
  void 	Scenari(); 
  void 	Settings(); 
  void	freeze(bool);
private:
// QTimer 	 *orologio;
  timeScript* dataOra;
int xClock,yClock,xTemp,yTemp;
  QPtrList<BtButton> elencoButtons;
  bool	freez;
};


#define MAX_BUT_SOTMEN 	9
#define DIM_BUT_HOME 		80 

#define OFFSET_CLOCK_1SEP	45
#define OFFSET_CLOCK_2SEP	102
#define OFFSET_CLOCK_MIN	55   
#define OFFSET_CLOCK_SEC	115
#define SPAZIO_CLOCK_DIGIT 	60
#define LUNG_CLOCK		175
   
#endif // BT_HOMEPAGE
