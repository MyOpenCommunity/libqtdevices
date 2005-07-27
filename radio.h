/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** radio.h
**
**definizioni della pagina di visualizzazione radio
**
****************************************************************/

#ifndef RADIO_H
#define RADIO_H

#include <qframe.h>
#include "btlabel.h"
#include <qprocess.h>
#include "btbutton.h"
#include "main.h"
#include "bannfrecce.h"
#include <qlcdnumber.h>
#include <qcursor.h>

/*!
  \class radio
  \brief This class implements the management of the FM tuner specific page.

  \author Davide
  \date lug 2005
*/      
class  radio : public QWidget
{
   Q_OBJECT
public:
    radio( QWidget *parent=0, const char *name=0 );
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
   BtButton *memoBut,*decBut,*aumBut,*autoBut,*manBut,*cicBut;
   BtButton *unoBut,*dueBut,*treBut,*quatBut,*cinBut,*cancBut;
   BtLabel* rdsLabel,* radioName,*progrText;
   QLCDNumber *freq;
/*!
  \brief Sets the name of the tuner found in user configuration file
*/         
   void 	setName(char*);
/*!
  \brief Gets the name of the tuner found in user configuration file
*/                 
   char* 	getName();
/*!
  \brief Sets the frequency of the syntonized station
*/            
   void 	setFreq(float);
/*!
  \brief Gets the frequency of the syntonized station
*/     
   float 	getFreq();
/*!
  \brief Sets the RDS message of the syntonized station
*/     
   void 	setRDS(char*);
/*!
  \brief Gets the RDS message of the syntonized station
*/     
   char* 	getRDS();    
/*!
  \brief Sets the station number of the syntonized station
*/            
   void 	setStaz(uchar);
/*!
  \brief Gets the station number of the syntonized station
*/     
   uchar 	getStaz();
/*!
  \brief Retrieves if the selected search method is automatic or manual
*/     
   bool 	isManual();
/*!
  \brief Draws the page
*/     
   void 	draw(); 
    
signals:
/*!
  \brief Emitted when the page is going to be closed
*/       
    void 	Closed();
/*!
  \brief Emitted when there's a request of an automatic search towards greater frequencies
*/     
    void 	aumFreqAuto();
/*!
  \brief Emitted when there's a request of an automatic search towards smaller frequencies
*/         
    void	decFreqAuto();
/*!
  \brief Emitted when there's a request of a manual search towards greater frequencies 
*/         
    void	aumFreqMan();
/*!
  \brief Emitted when there's a request of an manual search towards lesser frequencies
*/         
    void	decFreqMan();
/*!
  \brief Emitted to save the station actually tuned to one of the memorized stations
*/         
    void	memoFreq(uchar);
/*!
  \brief Emitted to switch to the next station
*/         
    void 	changeStaz();
/*!
  \brief Emitted to ask the frequency actually tuned
*/         
    void	richFreq();
/*!
  \brief Emitted to enter the  tuner details page
*/         
    void 	entroParam();
/*!
  \brief Emitted to exit the tuner details page
*/             
    void	escoParam();
    
public slots:
/*!
  \brief Shows the tuner details page 
*/             
    void showRadio();
/*!
  \brief Changes the state to automatic search
*/         
    void setAuto();
/*!
  \brief Changes the state to manual search
*/             
    void setMan();
/*!
  \brief saves the station actually tued to station number one
*/             
    void memo1();
/*!
  \brief saves the station actually tued to station number two
*/                 
    void memo2();
/*!
  \brief saves the station actually tued to station number three
*/                 
    void memo3();
/*!
  \brief saves the station actually tued to station number for
*/                 
    void memo4();
/*!
  \brief saves the station actually tued to station number five
*/                 
    void memo5();
/*!
  \brief Shows the memorization page
*/                 
    void cambiaContesto();
/*!
  \brief Hides -memorization page ans show back tuner details page
*/                 
    void ripristinaContesto();
/*!
  \brief At the end of a manual search ask the frequency tuned to the tuner to align to the visualized frequency
*/                 
    void verTas();

private:
    float	frequenza;
    uchar	stazione;
    char 	rds[9];
    char 	nome[15];
    bool 	manual,wasManual;
    bannFrecce * bannNavigazione;
};


#endif // RADIO_H
