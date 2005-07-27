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


/*!
  \class diffSonora
  \brief This is a class that manage sound diffusion function of the system. 
  
  It is composed of two subtree (sottoMenu): amplifiers and sources. In the first row of the page is shown the source active at a certain time while in the remaining rows there are all the amplifiers one ca control.
  \author Davide
  \date lug 2005
*/  
class diffSonora : public QWidget
{
    Q_OBJECT
public:
    diffSonora( QWidget *parent=0, const char *name=0);
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
 /*!
  \brief Adds an object in the class.
  
  If it is a source it is added to the sources subtree otherwise it's added to the amplifiers one. This method reproduce the one implemented in sottoMenù in similar manner.
*/       
   int 	addItem(char tipo= 0, char* nome=NULL , void* indirizzo=NULL ,char* IconaSx=NULL,char* IconaDx=NULL,char* IconaAttiva=NULL,char* IconaDisattiva=NULL,int periodo=0 , int numFrame=0);
 /*!
  \brief Sets the row's number.
*/       
   void 	setNumRighe(uchar);
 /*!
  \brief Initialize the class.
  
  It recall the sources' and amplifiers' inizialization procedure.
*/       
   void 	inizializza();
 /*!
  \brief Sets the geometry passing x,y,w,h.
*/       
   void	setGeom(int,int,int,int);
 /*!
  \brief Changes the navigation bar mode as described in bannFrecce
*/       
   void 	setNavBarMode(uchar);
   sottoMenu* amplificatori;  
 /*!
  \brief Recalls a draw for both amplifiers and sources.
*/       
   void 	draw();
 /*!
  \brief Recalls a forced draw for both amplifiers and sources.
  
  A forced draw force a draw of the subtree(sottoMenu) also if its internal index remain unchanged.
*/          
   void	forceDraw();
signals:
 /*!
  \brief Emitted when the object is closed.
*/          
    void 	Closed();
 /*!
  \brief Emitted when there are \a Open \a frame incoming to make them arrive to amplifiers and sources.
*/              
    void	gesFrame(char*);    
 /*!
  \brief Emitted when there are \a Open \a frame outcoming from amplifiers or sources to make them arrive to openClient.
*/                  
    void	sendFrame(char*);
 /*!
  \brief Emitted when there is a banner forcing (de)freezing state.
*/                      
    void 	freeze(bool);
 /*!
  \brief Emitted thought sources and amplifiers when the device is going into (de)freezing state.
*/                          
    void 	freezed(bool);
public slots:
 /*!
  \brief Analyze \a Open \a frame to detect which source is active at a certain time.
  
  When a new source is seen to be active, a change in source is forced an it's redrawn.
*/                          
    void 	gestFrame(char*);	
 /*!
  \brief Make sure a source being active before showing the page.
*/                      
    void	show();
 /*!
  \brief Make sure that amplifier index is resetted before hiding.
  
  Resetting amplifier index guarantees that nex time amplifiers will be shown the first banners will be drawn.
*/                          
    void 	hide();
 /*!
  \brief Emittes the closed signal when amplifiers subtree(sottoMenu) is closed.
*/                          
    void 	fineVis();
private:
  uchar numRighe,isVisual;	
  sottoMenu* sorgenti;

};


#endif // DIFFSONORA_H
