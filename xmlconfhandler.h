/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**xmlconfhandler.h
**
**definizioni per classe per il parsing SAX di conf.xml
**
****************************************************************/

#ifndef XMLCONFHANDLER_H
#define XMLCONFHANDLER_H   

#include <qxml.h>
#include "homepage.h"
#include "sottomenu.h"
#include "diffsonora.h"
#include "antintrusione.h"
#include "genericfunz.h"
#include "termoregolaz.h"
#include "btmain.h"
#include "versio.h"
//#include "openclient.h"

#include <qstring.h>
//class QString;

/*!
  \class xmlconfhandler
  \brief This class parses the user configuration file and generates all the objects required for system control.

  Using QT SAX handler for xml parsing, this class discoveres which system have to be created and which banners have to be instantiated and make them.
  Constructor argument description is as follow:
  1. pointer to \a BtMain object;
  2. pointer to the \a homePage pointer; the homePage is the page populated with buttons activating various functions; 
  3. pointer to \a specialPage pointer; the specialPage is the page with up to three rows with data/clock/temperatures and a special command; 
  4. pointer to \a lighting subtree (sottoMenu) pointer;
  5. pointer to \a scenarios subtree (sottoMenu) pointer;
  6. pointer to \a appliance's control subtree (sottoMenu) pointer;  
  7. pointer to \a settings subtree (sottoMenu) pointer;  
  8. pointer to \a automations subtree (sottoMenu) pointer;  
  9. pointer to \a thermoregulation subtree (sottoMenu) pointer;    
  10. pointer to \a sound \a diffusion subtree (sottoMenu) pointer;  
  11. pointer to \a anti-intusion subtree (sottoMenu) pointer;  
  12. pointer to \a default \a page subtree pointer. The default page is the page shown a minute after the last touch;  
  13. pointer to commands socket device;
  14. pointer to monitor socket device;  
  15. pointer to the page with versions;  
  16. pointer to \a schedulations subtree (sottoMenu) pointer;  
  17. pointer to \a background color;    
  18. pointer to \a foreground color;    
  19. pointer to \a second \a foreground color;    
  
  \author Davide
  \date lug 2005
*/  
class xmlconfhandler : public QXmlDefaultHandler
{
public:
    xmlconfhandler(BtMain *BtM=NULL, homePage**home=NULL,  homePage**specPage=NULL,  sottoMenu**scenari_evoluti=NULL, sottoMenu**illumino=NULL, sottoMenu**scenari=NULL, sottoMenu**carichi=NULL, sottoMenu**imposta=NULL, sottoMenu**automazioni=NULL, termoregolaz** termo=NULL, diffSonora**difSon=NULL, antintrusione** antintr=NULL, QWidget** pagDefault=NULL,Client * client_comandi=NULL, Client *  client_monitor=NULL, versio* datiGen=NULL,QColor* bg=NULL,QColor* fg1=NULL,QColor* fg2=NULL);
    
    ~xmlconfhandler();
/*!
  \brief Reset and initialize values
*/  
    bool startDocument();
/*!
  \brief take memory of the name of the tag which is actually parsed. It is also possible understand which is the level actually parsed (seek the firs empty tag!)
*/    
    bool startElement( const QString&, const QString&, const QString& , 
                       const QXmlAttributes& );
/*!
  \brief Saves the values read during the parsing in variables
*/      
    bool characters( const QString & ch );
 /*!
  \brief Makes the "sottoMenu", populate them and make connections and makes a certain number of initalization
*/      
    bool endElement( const QString&, const QString&, const QString& );

private:
    QString CurTagL1,CurTagL2,CurTagL3,CurTagL4,CurTagL5,CurTagL6,CurTagL7;
    bool ok;

    int hompage_isdefined;
    int hompage_type;
    int hompage_id;

    int sottomenu_id;
    QString sottomenu_descr;
    int sottomenu_left;
    int sottomenu_top;
    QString sottomenu_icon_name;
    QString sottomenu_where;

    int page_id;
    QString page_descr;

    int page_item_id;
    QString page_item_descr;
    QString page_item_what;
    QString page_item_where;
    QString page_icon, page_item_who, page_item_type, page_item_action;
    int page_item_softstart, page_item_softstop;

    void * page_item_indirizzo;
    
    QPtrList<QString> * page_item_list_img;
    QPtrList<QString> * page_item_list_group;
    QPtrList<QString> * page_item_list_txt;
    QPtrList<QString> * page_item_list_txt_times;
    
    scenEvo_cond *page_item_cond;
    QPtrList<scenEvo_cond> *page_item_cond_list;
    
      int	par1, par2,par3, par4;
      unsigned int itemNum;
      QString CurTagL4_copy;
    QColor Background, Foreground,SecondForeground;    
    unsigned char idPageDefault,car;

    void set_page_item_defaults();
    
    homePage **home;
    homePage **specPage;
    sottoMenu **illumino,**scenari,**carichi,**imposta,**automazioni,**sched,
	**scenari_evoluti;
    termoregolaz **termo;
    diffSonora **difSon;
    antintrusione** antintr;
    QWidget ** pagDefault;
    versio *datiGen ;

    BtMain * BtM;
    Client * client_comandi;
    Client *  client_monitor; 
};                   



#define IMG_PATH "cfg/skin/"
#endif 
