/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** diffsonora.h
**
**definizioni della pagina di sottoMenu diffusione Sonora
**
****************************************************************/

#ifndef DIFFSONORA_H
#define DIFFSONORA_H

#include <qwidget.h>
//#include "banner.h"
#include "items.h"
//#include "main.h"
#include "sottomenu.h"
#include "openclient.h"
#include <qptrlist.h> 

#include <qcursor.h>

//class QPushButton;


class diffSonora : public QWidget
{
    Q_OBJECT
public:
    diffSonora( QWidget *parent=0, const char *name=0);
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   int 	setBGPixmap(char* );
   int 	addItem(char tipo= 0, char* nome=NULL , void* indirizzo=NULL ,char* IconaSx=NULL,char* IconaDx=NULL,char* IconaAttiva=NULL,char* IconaDisattiva=NULL,int periodo=0 , int numFrame=0);
   void 	setNumRighe(uchar);
   void 	inizializza();
   void	setGeom(int,int,int,int);
   void 	setNavBarMode(uchar);
   sottoMenu* amplificatori;  
   void 	draw();
   void	forceDraw();
signals:
    void 	Closed();
    void	gesFrame(char*);    
    void	sendFrame(char*);
    void 	freeze(bool);
    void 	freezed(bool);
public slots:
    void 	gestFrame(char*);	
    void	show();
    void 	fineVis();
private:
  uchar numRighe,isVisual;	
  sottoMenu* sorgenti;

};


#endif // DIFFSONORA_H
