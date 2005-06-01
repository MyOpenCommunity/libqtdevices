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

class xmlconfhandler : public QXmlDefaultHandler
{
public:
    xmlconfhandler(BtMain *BtM=NULL, homePage**home=NULL,  homePage**specPage=NULL,  sottoMenu**illumino=NULL, sottoMenu**scenari=NULL, sottoMenu**carichi=NULL, sottoMenu**imposta=NULL, sottoMenu**automazioni=NULL, termoregolaz** termo=NULL, diffSonora**difSon=NULL, antintrusione** antintr=NULL, QWidget** pagDefault=NULL,Client * client_comandi=NULL, Client *  client_monitor=NULL, versio* datiGen=NULL,sottoMenu** sched=NULL,QColor* bg=NULL,QColor* fg1=NULL,QColor* fg2=NULL);
    
    ~xmlconfhandler();

    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& , 
                       const QXmlAttributes& );
    bool characters( const QString & ch );
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
    QString page_icon, page_item_who, page_item_type;

    void * page_item_indirizzo;
    
    QPtrList<QString> * page_item_list_img;
    QPtrList<QString> * page_item_list_group;
    QPtrList<QString> * page_item_list_txt;
    
      int	par1, par2,par3;
      unsigned int itemNum;
      QString CurTagL4_copy;
    QColor Background, Foreground,SecondForeground;    
    unsigned char idPageDefault,car;

    void set_page_item_defaults();
    
    homePage **home;
    homePage **specPage;
    sottoMenu **illumino,**scenari,**carichi,**imposta,**automazioni,**sched;
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
