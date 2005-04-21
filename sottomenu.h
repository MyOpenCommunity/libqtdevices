/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** sottomenu.h
**
**definizioni della pagine di sottoMenu
**
****************************************************************/

#ifndef SOTTOMENU_H
#define SOTTOMENU_H

#include <qwidget.h>
//#include <qlabel.h>
#include "banner.h"
#include <qptrlist.h> 
#include "main.h"
#include "bannfrecce.h"
#include <stdlib.h>

//class QPushButton;


class sottoMenu : public QWidget
{
    Q_OBJECT
public:
    sottoMenu( QWidget *parent=0, const char *name=0, uchar withNavBar=3 ,int width=MAX_WIDTH,int  height=MAX_HEIGHT,uchar n=NUM_RIGHE-1);
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );		
   void 	setBGColor(QColor );
   void 	setFGColor(QColor );	
   int 	setBGPixmap(char* );
   int 	addItem(char tipo= 0, char* nome=NULL , void* indirizzo=NULL ,char* IconaSx=NULL,char* IconaDx=NULL,char* IconaAttiva=NULL,\
		char* IconaDisattiva=NULL,int periodo=0 , int numFrame=0, QColor  secondFroreground=QColor(0,0,0) , char* descr1=NULL,\
		char* descr2=NULL,char* descr3=NULL,char* descr4=NULL);
   void 	inizializza();
   void 	draw();
   void 	setNumRighe(uchar);
  banner* getLast();
  bool 	setPul(char* chi="", char* where="");
  bool	setGroup(char* chi="", char* where="", bool group[9] =NULL);// {(bool)FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE});
  void 	setIndex(char*);
  void 	setNumRig(uchar);
  void 	setHeight(int);
  uchar 	getNumRig();
  int 	getHeight();  
  void 	setNavBarMode(uchar=0, char* IconBut4=ICON_FRECCIA_DX);
  void setGeometry(int, int, int, int );
signals:
  void 	Closed();
  void 	gestFrame(char*);
  void 	sendFrame(char*);
  void 	richStato(char*);
  void 	rispStato(char*);
  void	freeze(bool);
  void 	frez(bool);
  void	goDx();  
public slots:
  void 	goUp();	
  void 	goDown();
  void	init();    
  void 	freezed(bool);
//  void 	show();
//  void 	mousePressEvent ( QMouseEvent * );
  void 	mouseReleaseEvent ( QMouseEvent * );  
  void 	killBanner(banner*);
private:
  QPtrList<banner> elencoBanner;
  QTimer* iniTim;
  int indice,height,width, indicold;  
  uchar numRighe, hasNavBar;	
  bannFrecce * bannNavigazione;
  bool 	freez;
};


#endif // SOTTOMENU_H
