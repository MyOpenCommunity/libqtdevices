/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** antintrusione.h
**
**definizioni della pagina di sottoMenu antiintrusione
**
****************************************************************/

#ifndef ANTINTRUS_H
#define ANTINTRUS_H

#include <qwidget.h>
#include "items.h"
#include "sottomenu.h"
#include "openclient.h"
#include "banner.h"
#include <qptrlist.h> 



class antintrusione : public QWidget
{
    Q_OBJECT
public:
    antintrusione( QWidget *parent=0, const char *name=0);
   void 	setBGColor(int, int , int );
   void 	setFGColor(int , int , int );
   int 	setBGPixmap(char* );
   int 	addItem(char tipo= 0, char* nome=NULL , void* indirizzo=NULL ,char* IconaSx=NULL,char* IconaDx=NULL,char* IconaAttiva=NULL,char* IconaDisattiva=NULL,int periodo=0 , int numFrame=0);
   void 	setNumRighe(uchar);
   void 	inizializza();
   void	setGeom(int,int,int,int);
   void 	setNavBarMode(uchar);
      void draw();
signals:
    void 	Closed();
    void	gestFrame(char*);    
    void	sendFrame(char*);
    void 	freeze(bool);
    void 	freezed(bool);
public slots:
    void 	gesFrame(char*);	
    void 	ctrlAllarm();
    void 	testranpo();
private:
  uchar numRighe;	
  sottoMenu* zone;  
  sottoMenu* impianto;
  sottoMenu* allarmi;
};


#endif // ANTINTRUS_H
