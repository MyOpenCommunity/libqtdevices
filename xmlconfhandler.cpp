/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**xmlconfhandler.cpp
**
**classe per il parsing SAX di conf.xml
**
****************************************************************/


#include "xmlconfhandler.h"
#include "main.h"
#include "homepage.h"
#include "sottomenu.h"
#include "impostatime.h"
#include "diffsonora.h"
#include "sveglia.h"
#include "genericfunz.h"


#include <stdio.h>
#include <qstring.h>

unsigned char tipoData=0;

/*******************************************
*
*******************************************/
xmlconfhandler::xmlconfhandler(BtMain *BM, homePage**h, homePage**sP,  sottoMenu**i, sottoMenu**s,sottoMenu**c, sottoMenu**im,  sottoMenu**a, termoregolaz** t,\
                               diffSonora**dS, antintrusione** ant,QWidget** pD,Client * c_c, Client *  c_m,versio* dG, sottoMenu** sch,\
                               QColor* bg, QColor* fg1, QColor *fg2)
{
    home=h;
    specPage=sP;
    illumino=i;
    scenari=s;
    carichi=c;
    imposta=im;
    automazioni=a;
    termo=t;
    difSon=dS;
    antintr=ant;
    pagDefault=pD;
    BtM=BM;
    client_comandi=c_c;
    client_monitor=c_m;
    datiGen=dG;
    sched = sch;
    
 //   bg_r, bg_g, bg_b,fg_r,fg_g, fg_b,fg_r1, fg_g1, fg_b1;
    Background=*bg;
    Foreground=*fg1;
    SecondForeground=*fg2;//QColor(255,0,0);
    
    page_item_list_img = new QPtrList<QString>;
    page_item_list_group = new QPtrList<QString>;
    page_item_list_txt = new QPtrList<QString>;
}

/*******************************************
*
*******************************************/
xmlconfhandler::~xmlconfhandler()
{
    delete page_item_list_img;
    delete page_item_list_group;
    delete page_item_list_txt;
}


/*******************************************
*
* Inizio nuovo documento xml
*
*
*******************************************/
void xmlconfhandler::set_page_item_defaults()
{
    page_item_id=0;
    // page_item_descr="";
    page_item_indirizzo=NULL;
    
    page_item_list_img->clear();
    page_item_list_group->clear();
    par1=par2=0;
    page_item_list_img->clear();
    page_item_list_txt->clear();
    page_item_list_group->clear(); 
    page_item_what = "";
    page_item_descr = "";
    page_item_what = "";
    page_item_where = "";
    page_icon = "";
    page_item_who = "";
    page_item_type = "0";
    itemNum=0;
}


/*******************************************
*
* Inizio nuovo documento xml
*
*
*******************************************/
bool xmlconfhandler::startDocument()
{
    CurTagL1 = "";
    CurTagL2 = "";
    CurTagL3 = "";
    CurTagL4 = "";
    CurTagL5 = "";
    CurTagL6 = "";
    CurTagL7 = "";
    sottomenu_icon_name = "";
    sottomenu_id=0;
    ok=0;
    hompage_isdefined=0;
    hompage_type=0;
    hompage_id=0;
    
    set_page_item_defaults();

    datiGen->setPaletteBackgroundColor(Background);
    
    return TRUE;
}

/*******************************************
*
* Letta nuova tag
*
*
*******************************************/
bool xmlconfhandler::startElement( const QString&, const QString&, 
                                   const QString& qName, 
                                   const QXmlAttributes& )
{
    car=0;
    
    //  qDebug("%s\n",(const char*)qName );
    if (CurTagL1.isEmpty())
    {
        CurTagL1 = qName;
        //	qDebug("ENTRO-1:%s",(const char*)CurTagL1);
    }
    else if (CurTagL2.isEmpty())
    {
        CurTagL2 = qName;
        //	qDebug("ENTRO-2:%s",(const char*)CurTagL2);
    }
    else if (CurTagL3.isEmpty()) 
    {
        CurTagL3 = qName;
        //	qDebug("ENTRO-3:%s",(const char*)CurTagL3);
    }
    else if (CurTagL4.isEmpty()) 
    {
        CurTagL4 = qName;
        //	qDebug("ENTRO-4:%s",(const char*)CurTagL4);
    }
    else if (CurTagL5.isEmpty()) 
    {
        CurTagL5 = qName;
        //	qDebug("ENTRO-5:%s",(const char*)CurTagL5);
    }
    else if (CurTagL6.isEmpty()) 
    {
        CurTagL6 = qName;
        //	qDebug("ENTRO-6:%s",(const char*)CurTagL6);
    }
    else if (CurTagL7.isEmpty())
    {
        CurTagL7 = qName;
        //    qDebug("ENTRO-7:%s\n",(const char*)CurTagL7);
    }
    
    return TRUE;
}

/*******************************************
*
* Esco da un livello
*
*
*******************************************/
bool xmlconfhandler::endElement( const QString&, const QString&, const QString& )
{
    if (!car)
        characters( QString("\000"));        // se ho un tag vuoto riempio con '\000' il suo campo 
    
    
    if (!CurTagL1.compare("configuratore"))
    {
            if (!CurTagL2.compare("displaypages"))
            {
                if ( !CurTagL3.compare("homepage") && (CurTagL4.isEmpty()) )
                {
                    /*		qWarning("HOMEPAGE");
                qWarning("hompage_isdefined=%d",hompage_isdefined);
                qWarning("hompage_id=%d",hompage_id);	*/
                    if (hompage_isdefined)
                    {
                        if (hompage_isdefined)
                            idPageDefault=hompage_id;
                        else
                            idPageDefault=0xFF;
                    }
                }
                else if (!CurTagL3.compare("pagemenu"))
                {
                    if ( CurTagL4.startsWith("item") && CurTagL5.isEmpty() )
                    {
                        /*		    qWarning("sottomenu_id=%d",sottomenu_id);
                    qWarning("sottomenu_descr=%s",(const char*)sottomenu_descr);
                    qWarning("sottomenu_left=%d",sottomenu_left);
                    qWarning("sottomenu_top=%d",sottomenu_top);
                    qWarning("sottomenu_icon_where=%s",(const char*)sottomenu_where);	  	  
                    qWarning("sottomenu_icon_name=%s",(const char*)sottomenu_icon_name);	*/
                        
                        
                        switch (sottomenu_id)
                        {
                        case AUTOMAZIONE:    
                        case ILLUMINAZIONE:
                        case ANTIINTRUSIONE:
                        case CARICHI:
                        case TERMOREGOLAZIONE:
                        case DIFSON:
                        case SCENARI:
                        case SCHEDULAZIONI:
                            case IMPOSTAZIONI: 				  // addbutton normali
                            (*home)->addButton(sottomenu_left,sottomenu_top,(char *)sottomenu_icon_name.ascii(), (char)sottomenu_id); break;
                            //  Home->addButton(160,125,ICON_IMPOSTAZIONI_80 ,IMPOSTAZIONI);
                            //			qWarning("ADDBUTTON NORMALE");
                            break;
                        case DATA:
                            (*home)->addDate(sottomenu_left+10,sottomenu_top+10,220,60,Background,Foreground,QFrame::Plain,3);
                            //			qWarning("ADDBUTTON DATA");
                            break;
                        case OROLOGIO:
                            (*home)->addClock(sottomenu_left+10,sottomenu_top+10,220,60,Background,Foreground,QFrame::Plain,3);
                            //			qWarning("ADDBUTTON OROLOGIO");
                            break;
                        case TEMPERATURA:
                            (*home)->addTemp((char *)sottomenu_where.ascii(),sottomenu_left+10,sottomenu_top+10,220,60,Background,Foreground,QFrame::Plain,3,"");
                            //			qWarning("ADDBUTTON TEMPERATURA");
                            break;
                        } // switch (sottomenu_id)
                        
                        
                        
                        // PREPARO PER QUELLA DOPO
                        sottomenu_descr="";
                        sottomenu_left=0;
                        sottomenu_top=0;
                        sottomenu_where="";
                        sottomenu_icon_name = "";
                        
                    } // if (!CurTagL4.startsWith("item"))
                } // if (!CurTagL3.compare("pagemenu"))
                else if (CurTagL3.startsWith("page"))
                {
                    //
                    // Aggiungo gli item delle pagine
                    //
                    
                    
                    if ( ( CurTagL4.startsWith("item") || !CurTagL4.compare("command") ) && CurTagL5.isEmpty() )
                    {
                        sottoMenu *pageAct=NULL;
                        qWarning("ESCO PAGEITEM:ID %d",page_item_id);
                        /*		    for ( QString * MyPnt = page_item_list_img->first(); MyPnt; MyPnt= page_item_list_img->next() )
                        qWarning("IMG=%s su %d",MyPnt->ascii(),page_item_list_img->count());*/
                        
                        for(int idx=page_item_list_img->count();idx<7;idx++)
                        {
                            page_item_list_img->append(new QString(""));
                        }
                        for(int idx=page_item_list_txt->count();idx<4;idx++)
                        {
                            page_item_list_txt->append(new QString(""));
                        }		     
                        /*		    for (QString * MyPnt = page_item_list_group->first(); MyPnt; MyPnt = page_item_list_group->next() )
                        qWarning("GROUP=%s",MyPnt->ascii());	*/
                        
                        // QString* pip;   
                        char pip[50];
                        memset(&pip[0],'\000',sizeof(pip));
                        void* pnt;
                        switch (  page_id )
                        {	    
                        case AUTOMAZIONE:
                            pageAct= (*automazioni);	
                            //         qDebug("automaz");
                            
                                case  ILLUMINAZIONE:
                            if (!pageAct)
                            {
                                pageAct= (*illumino); 
                                //       qDebug("illumino");
                            }
                        case CARICHI:
                            if (!pageAct)
                            {
                                pageAct=(*carichi); 		
                                //          qDebug("carichi");
                            }
                        case TERMOREGOLAZIONE:
                            if (!pageAct)
                            {
                                pageAct=(*termo);
                                //         qDebug("termo");
                            }
                        case  SCENARI:
                            if (!pageAct)
                            {
                                pageAct=(*scenari); 		
                                //             qDebug("scenari");
                            }
                        case IMPOSTAZIONI:
                            if (!pageAct)
                            {
                                pageAct=(*imposta);
                                //             qDebug("imposta");
                            }
                             case SCHEDULAZIONI:
                            if (!pageAct)
                            {
                                pageAct=(*sched);
                                //                 qDebug("schedul");
                            }
                            
                            if ( (!page_item_what.isNull()) && (!page_item_what.isEmpty())  )
                            {			       
                                strcpy(&pip[0], page_item_what.ascii());
                                strcat(pip,"*");
                                strcat(pip,page_item_where.ascii());
                            }
                            else
                                strcpy(&pip[0], page_item_where.ascii());
                            
                            if (page_item_list_group->isEmpty())
                            {
                                    pnt=pip;
                                }
                            else
                            {
                                    pnt=page_item_list_group;
                                }
                            if ((char)page_item_id==SET_SVEGLIA) 
                            {
                                    if  (par2==DI_SON) 
                                    {
                                        pnt=*difSon;        
                                    }
                                    else
                                    {
                                        pnt=NULL;
                                    }
                                } 
                            else if ((char)page_item_id==VERSIONE) 
                            {
                                    pnt=datiGen;
                                }
                            pageAct->addItem ((char)page_item_id, (char*)page_item_descr.ascii(), pnt/*(char*)pip.ascii() (char*)page_item_where.ascii()*/, (char*)page_item_list_img->at(0)->ascii(), (char*)page_item_list_img->at(1)->ascii() ,  (char*)page_item_list_img->at(2)->ascii(),  (char*)page_item_list_img->at(3)->ascii(),  par1,  par2, SecondForeground,  (char*)page_item_list_txt->at(0)->ascii(),   (char*)page_item_list_txt->at(1)->ascii(),  (char*)page_item_list_txt->at(2)->ascii(),  (char*)page_item_list_txt->at(3)->ascii(),   (char*)page_item_list_img->at(4)->ascii(),    (char*)page_item_list_img->at(5)->ascii(),  (char*)page_item_list_img->at(6)->ascii() , par3 )  ;
                            break;			   
                        case ANTIINTRUSIONE:
                            //     qDebug("antiintr");
                            (*antintr)->addItem ((char)page_item_id, (char*)page_item_descr.ascii(),  (char*)page_item_where.ascii(), (char*)page_item_list_img->at(0)->ascii(), (char*)page_item_list_img->at(1)->ascii() ,  (char*)page_item_list_img->at(2)->ascii(),  (char*)page_item_list_img->at(3)->ascii(),  par1,  par2)  ;
                            break;
                               case DIFSON:
                            //      qDebug("difson");
                            if ( (!page_item_what.isNull()) && (!page_item_what.isEmpty())  )
                            {			       
                                strcpy(&pip[0], page_item_what.ascii());
                                strcat(pip,"*");
                                strcat(pip,page_item_where.ascii());
                            }
                            else
                                strcpy(&pip[0], page_item_where.ascii());
                            
                            if (page_item_list_group->isEmpty())
                            {
                                    pnt=pip;
                                }
                            else
                            {
                                    pnt=page_item_list_group;
                                }			
                            
                            (*difSon)->addItem ((char)page_item_id, (char*)page_item_descr.ascii(),  pnt/*(char*)page_item_where.ascii()*/, (char*)page_item_list_img->at(0)->ascii(), (char*)page_item_list_img->at(1)->ascii() ,  (char*)page_item_list_img->at(2)->ascii(),  (char*)page_item_list_img->at(3)->ascii(),  par1,  par2)  ;
                            break;
                        case SPECIAL:
                            //       qDebug("special");
                            switch(page_item_id)
                            {
                            case TEMPERATURA:
                                (*specPage) ->addTemp((char*)page_item_where.ascii(),10,(itemNum-1)*80+10,220,60,Background,Foreground,(int)QFrame::Plain,3,(char*)page_item_descr.ascii());
                                break;
                            case DATA:
                                //sottomenu_left,sottomenu_top,
                                (*specPage) ->addDate(10,(itemNum-1)*80+10,220,60,Background,Foreground,QFrame::Plain,3);
                                break;
                            case OROLOGIO:
                                //sottomenu_left,sottomenu_top,
                                (*specPage) ->addClock(10,(itemNum-1)*80+10,220,60,Background,Foreground,QFrame::Plain,3);	
                                break;
                            case CMDSPECIAL:
                                
                                (*specPage) ->addButton(60,260,(char*)page_item_list_img->at(0)->ascii(),SPECIAL,(char*)page_item_who.ascii(),(char*)page_item_what.ascii(),(char*)page_item_where.ascii(),(char)page_item_type.toInt( &ok, 10 ));
                                (*specPage) ->addDescr((char*)page_item_descr.ascii(), 60,240,180,20,Background,Foreground,QFrame::Plain,3);
                                pageAct=NULL;
                                break;
                            }
                        }
                        
                        set_page_item_defaults();
                    } // if ( CurTagL4.startsWith("item") && CurTagL5.isEmpty() )
                    else if ( CurTagL3.startsWith("page") && CurTagL4.isEmpty() )
                    {
                        // Esco dalla pagina
                        //		    qWarning("DRAW %s",(const char*)page_descr);
                        
                        
                        switch (page_id)
                        {
                        case AUTOMAZIONE:
                            //			qWarning(".- .- .- -. -.  .- -.QObject::connect AUTOMAZIONE");
                            (*automazioni)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                    
                            QObject::connect(*home,SIGNAL(Automazione()),*automazioni,SLOT(showFullScreen()));
                            QObject::connect(*automazioni,SIGNAL(Closed()),*home,SLOT(showFullScreen()));                        
#endif                        
#if !defined (BTWEB) && !defined (BT_EMBEDDED)                    
                            QObject::connect(*home,SIGNAL(Automazione()),*automazioni,SLOT(show()));
                            QObject::connect(*automazioni,SIGNAL(Closed()),*home,SLOT(show()));                        
#endif                              
                            QObject::connect(*automazioni,SIGNAL(Closed()),*automazioni,SLOT(hide()));
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*automazioni,SIGNAL(gestFrame(char *)));
                            QObject::connect(*automazioni,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*automazioni,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*automazioni,SLOT(freezed(bool)));
                            //(*automazioni)->inizializza();
                            break;
                        case ILLUMINAZIONE:
                            //			qWarning("-. .- . -. - -. .-. -QObject::connect ILLUMINAZIONE");
                            (*illumino)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                       
                            QObject::connect(*home,SIGNAL(Illuminazione()),*illumino,SLOT(showFullScreen()));
                            QObject::connect(*illumino,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)       
                            QObject::connect(*home,SIGNAL(Illuminazione()),*illumino,SLOT(show()));
                            QObject::connect(*illumino,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                          
                            QObject::connect(*illumino,SIGNAL(Closed()),*illumino,SLOT(hide()));			
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*illumino,SIGNAL(gestFrame(char *)));
                            QObject::connect(*illumino,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*illumino,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            //     QObject::QObject::connect(client_comandi,SIGNAL(frameIn(char *)),&illumino,SIGNAL(gestFrame(char *)));
                            QObject::connect(*illumino,SIGNAL(richStato(char *)),client_comandi,SLOT(richStato(char *)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*illumino,SLOT(freezed(bool)));
                            //(*illumino)->inizializza();
                            break;
                        case ANTIINTRUSIONE:
                            //			qWarning(".- .- .- .- .-.- - -.QObject::connect ANTIINTRUSIONE");
                            (*antintr)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Antiintrusione()),*antintr,SLOT(showFullScreen()));
                            QObject::connect(*antintr,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)         
                            QObject::connect(*home,SIGNAL(Antiintrusione()),*antintr,SLOT(show()));
                            QObject::connect(*antintr,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                       
                            QObject::connect(*antintr,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*antintr,SLOT(gesFrame(char *)));    
                            QObject::connect(*antintr,SIGNAL(Closed()),*antintr,SLOT(hide()));
                            
                            QObject::connect(*antintr,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*antintr,SIGNAL(freezed(bool)));
                            //(*antintr)->inizializza();
                            break;
                        case CARICHI:
                            //			qWarning(".- .- .- .- .-  -. .- .- .- QObject::connect CARICHI");
                            (*carichi)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Carichi()),*carichi,SLOT(showFullScreen()));
                            QObject::connect(*carichi,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)         
                            QObject::connect(*home,SIGNAL(Carichi()),*carichi,SLOT(show()));
                            QObject::connect(*carichi,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                        
                            QObject::connect(*carichi,SIGNAL(Closed()),*carichi,SLOT(hide()));			
                            QObject::connect(*carichi,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*carichi,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*carichi,SLOT(freezed(bool)));
                            break;
                        case TERMOREGOLAZIONE:
                            //			qWarning(" - -  .     .- . . .- .-QObject::connect TERMOREGOLAZIONE ");
                            (*termo)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Termoregolazione()),*termo,SLOT(showFullScreen()));
                            QObject::connect(*termo,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)    
                            QObject::connect(*home,SIGNAL(Termoregolazione()),*termo,SLOT(show()));
                            QObject::connect(*termo,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                    
                            QObject::connect(*termo,SIGNAL(Closed()),*termo,SLOT(hide()));
                            
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*termo,SIGNAL(gestFrame(char *)));
                            QObject::connect(*termo,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*termo,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            //(*termo)->inizializza();
                            break;
                        case DIFSON:
                            //			qWarning("- - -. .-  .- .- .- .- .- QObject::connect DIFSON");
                            (*difSon)->draw(); 
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Difson()),*difSon,SLOT(showFullScreen()));
                            QObject::connect(*difSon,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)     
                            QObject::connect(*home,SIGNAL(Difson()),*difSon,SLOT(show()));
                            QObject::connect(*difSon,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                      
                            QObject::connect(*difSon,SIGNAL(Closed()),*difSon,SLOT(hide()));
                            
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*difSon,SLOT(gestFrame(char *)));
                            QObject::connect(*difSon,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*difSon,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*difSon,SIGNAL(freezed(bool)));
                            //(*difSon)->inizializza();
                            break;
                        case SCENARI:
                            //			qWarning("- - -  - - - - - - - -QObject::connect ");
                            (*scenari)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Scenari()),*scenari,SLOT(showFullScreen()));
                            QObject::connect(*scenari,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)  
                            QObject::connect(*home,SIGNAL(Scenari()),*scenari,SLOT(show()));
                            QObject::connect(*scenari,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                       
                            QObject::connect(*scenari,SIGNAL(Closed()),*scenari,SLOT(hide()));
                            
                            QObject::connect(*scenari,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*scenari,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*scenari,SLOT(freezed(bool)));
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*scenari,SIGNAL(gestFrame(char *)));
                            //(*scenari)->inizializza();
                            break;
                        case IMPOSTAZIONI:    
                            qWarning("- - - - - - - - - - - QObject::connect IMPOSTAZIONI");
                            (*imposta)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Settings()),*imposta,SLOT(showFullScreen()));
                            QObject::connect(*imposta,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)                  
                            QObject::connect(*home,SIGNAL(Settings()),*imposta,SLOT(show()));
                            QObject::connect(*imposta,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                          
                            QObject::connect(*imposta,SIGNAL(Closed()),*imposta,SLOT(hide()));
                            
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*imposta,SIGNAL(gestFrame(char *)));
                            QObject::connect(*imposta,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*imposta,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(*imposta,SIGNAL(freeze(bool)),BtM,SLOT(freezed(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*imposta,SLOT(freezed(bool)));
                            break;
                        case SCHEDULAZIONI:    
                            //			qWarning("- - - - - - - - - - - QObject::connect IMPOSTAZIONI");
                            (*sched)->draw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*home,SIGNAL(Schedulazioni()),*sched,SLOT(showFullScreen()));
                            QObject::connect(*sched,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)      
                            QObject::connect(*home,SIGNAL(Schedulazioni()),*sched,SLOT(show()));
                            QObject::connect(*sched,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                    
                            QObject::connect(*sched,SIGNAL(Closed()),*sched,SLOT(hide()));
                            
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*sched,SIGNAL(gestFrame(char *)));
                            QObject::connect(*sched,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
                            QObject::connect(*sched,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                            QObject::connect(*sched,SIGNAL(freeze(bool)),BtM,SLOT(freezed(bool)));
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*sched,SLOT(freezed(bool)));
                            break;
                        case SPECIAL:    
                            //			qWarning("-- - - - - - -- - - QObject::connect SPECIAL");
                            //(*specPage)->draw();
                            QObject::connect(BtM,SIGNAL(freeze(bool)),*specPage,SLOT(freezed(bool)));          
                            QObject::connect(*specPage,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));     
                            QObject::connect(*specPage,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));  
                            QObject::connect(*specPage,SIGNAL(Close()),*specPage,SLOT(hide()));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
                            QObject::connect(*specPage,SIGNAL(Close()),*home,SLOT(showFullScreen()));     
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)  
                            QObject::connect(*specPage,SIGNAL(Close()),*home,SLOT(show()));
#endif                                           
                            QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*specPage,SLOT(gestFrame(char *)));
                            break;
                        } // switch page_id
                        
                        // Pulisco per il prox
                        set_page_item_defaults();
                        page_id=0;
                        page_descr="";
                    }
                } //  if (CurTagL3.startsWith("page"))
                if ( !CurTagL2.compare("displaypages") && CurTagL3.isEmpty() )
                {
                    //		qWarning("QObject::connect HOME");
                    QObject::connect(BtM,SIGNAL(freeze(bool)),*home,SLOT(freezed(bool)));
                    QObject::connect(*home,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
                }
            } //  if (!CurTagL2.compare("displaypages"))
        } // if (!CurTagL1.compare("configuratore"))
    
    
    //
    //
    //
    if (!CurTagL7.isEmpty())
    {
            //    qDebug("ESCO-7:%s\n",(const char*)CurTagL7);
            CurTagL7 = "";
        }
    else if (!CurTagL6.isEmpty())
    {
            //	qDebug("ESCO-6:%s",(const char*)CurTagL6);
            CurTagL6 = "";
        }
    else if (!CurTagL5.isEmpty())
    {
            //	qDebug("ESCO-5:%s",(const char*)CurTagL5);
            CurTagL5 = "";
        }
    else if (!CurTagL4.isEmpty())
    {
            //	qDebug("ESCO-4:%s",(const char*)CurTagL4);
            CurTagL4 = "";
        }
    else if (!CurTagL3.isEmpty())
    {
            //	qDebug("ESCO-3:%s",(const char*)CurTagL3);
            CurTagL3 = "";
        }
    else if (!CurTagL2.isEmpty())
    {
            //	qDebug("ESCO-2:%s",(const char*)CurTagL2);
            CurTagL2 = "";
        }
    else if (!CurTagL1.isEmpty())
    {
            //	qDebug("ESCO-1:%s",(const char*)CurTagL1);
            CurTagL1 = "";
        }
    
    
    return TRUE;
}

/*******************************************
*
* Letto nuovo valore
*
*
*******************************************/
bool xmlconfhandler::characters( const QString & qValue)
{
    car=1;
    
    if (!CurTagL1.compare("configuratore"))
    {
        if (!CurTagL2.compare("displaypages"))
        {
            if (!CurTagL3.compare("orientation"))
            {   	    
                setOrientation((unsigned char)qValue.ascii()); 	
            }
            /*else if (!CurTagL3.compare("bg"))
            {   
                if (!CurTagL4.compare("r"))
                {   
                    bg_r=qValue.toInt( &ok, 10 );
                }
                else if (!CurTagL4.compare("g"))
                {   
                    bg_g=qValue.toInt( &ok, 10 );
                }
                else if (!CurTagL4.compare("b"))
                {   
                    bg_b=qValue.toInt( &ok, 10 );
                    datiGen->setPaletteBackgroundColor(QColor(bg_r,bg_g,bg_b));
                }
            }
            else if (!CurTagL3.compare("fg1"))
            {   
                if (!CurTagL4.compare("r"))
                {   
                    fg_r=qValue.toInt( &ok, 10 );
                }
                else if (!CurTagL4.compare("g"))
                {   
                    fg_g=qValue.toInt( &ok, 10 );
                }
                else if (!CurTagL4.compare("b"))
                {   
                    fg_b=qValue.toInt( &ok, 10 );
                    datiGen->setPaletteForegroundColor(QColor(fg_r,fg_g,fg_b));
                }
            }
            else if (!CurTagL3.compare("fg2"))
            {   
                if (!CurTagL4.compare("r"))
                {   
                    fg_r1=qValue.toInt( &ok, 10 );
                }
                else if (!CurTagL4.compare("g"))
                {   
                    fg_g1=qValue.toInt( &ok, 10 );
                }
                else if (!CurTagL4.compare("b"))
                {   
                    fg_b1=qValue.toInt( &ok, 10 );
                    SecondForeground=QColor(fg_r1,fg_g1,fg_b1);
                }
                
            }*/
            //
            // Leggo info homepage
            //
            else if (!CurTagL3.compare("homepage"))
            {
                if (!CurTagL4.compare("isdefined"))
                    hompage_isdefined = qValue.toInt( &ok, 10 );
                else if (!CurTagL4.compare("type"))
                    hompage_type = qValue.toInt( &ok, 10 );
                else if (!CurTagL4.compare("id"))
                    hompage_id = qValue.toInt( &ok, 10 );
            }
            //
            // Leggo info pagemenu 
            //
            else if (!CurTagL3.compare("pagemenu"))
            {
                //        qDebug( "SONO DENTRO PAGEMENU: %s\n",(const char*)CurTagL4);
                //
                // Leggo tutti i tag che iniziano per item
                //
                if (CurTagL4.startsWith("id"))
                {	      
                        *home = new homePage(NULL,"homepage",Qt::WType_TopLevel | Qt::WStyle_Maximize | Qt::WRepaintNoErase);    
                        (*home)->setBGColor(Background.red(),Background.green(),Background.blue());
                        (*home)->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
                        QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*home,SLOT(gestFrame(char *)));
                    }
                
                else if (CurTagL4.startsWith("item"))
                {
                        if (!CurTagL5.compare("id"))
                        {
                            sottomenu_id = qValue.toInt( &ok, 10 );  
                            //			    qWarning("PAGEMENU:ID %d",sottomenu_id);
                            
                        } else if (!CurTagL5.compare("descr"))
                            sottomenu_descr = qValue;
                        else if (!CurTagL5.compare("left"))
                            sottomenu_left = qValue.toInt( &ok, 10 );  
                        else if (!CurTagL5.compare("top"))
                            sottomenu_top = qValue.toInt( &ok, 10 );
                        else if (!CurTagL5.compare("cimg1"))
                        {
                                sottomenu_icon_name = QString(IMG_PATH);
                                sottomenu_icon_name.append(qValue.ascii());
                                //				qDebug ("PAGEMENU:icon_name %s",sottomenu_icon_name.ascii());
                            } else if (!CurTagL5.compare("where"))
                                sottomenu_where = qValue;
                            
                        } // if (!CurTagL4.startsWith("item"))
            } // if (!CurTagL3.startsWith("pagemenu"))
            //
            // Leggo info pagine
            //
            else if (CurTagL3.startsWith("page"))
            {
                if (!CurTagL4.compare("id"))
                {
                            QWidget* pageAct;  
                            page_id = qValue.toInt( &ok, 10 );
                            qWarning("PAGE:ID %d",page_id);
                            
                            
                            switch (page_id)
                            {
                            case AUTOMAZIONE:	      
                                *automazioni = new sottoMenu (NULL,"AUTOM");
                                (*automazioni)->setBGColor(Background);
                                (*automazioni)->setFGColor(Foreground);
                                //              automazioni->hide();
                                pageAct=*automazioni;
                                //				qWarning("AUTOMAZIONE new.- . . .- -. -. .-");
                                break;
                            case ILLUMINAZIONE:	    
                                *illumino = new sottoMenu (NULL,"ILLUMINO");
                                (*illumino)->setBGColor(Background);
                                (*illumino)->setFGColor(Foreground);
                                //              illumino->hide();
                                pageAct=*illumino;		      
                                //				qWarning("ILLUMINAZIONE new.- .- .- .-  .--. ");
                                break;
                            case SCENARI:
                                *scenari = new sottoMenu (NULL,"SCENARI");
                                (*scenari)->setBGColor(Background);
                                (*scenari)->setFGColor(Foreground);
                                //              scenari->hide();
                                pageAct=*scenari;
                                //				qWarning("SCENARI new.- .- -. -. . -. -");
                                break;
                            case CARICHI:
                                *carichi = new sottoMenu (NULL,"CARICHI");
                                (*carichi)->setBGColor(Background);
                                (*carichi)->setFGColor(Foreground);
                                //              carichi->hide();
                                pageAct=*carichi;
                                //				qWarning("CARICHI new-. -. .- .-");
                                break;
                            case DIFSON:
                                *difSon = new diffSonora (NULL,"DIFSON");
                                (*difSon)->setBGColor(Background.red(),Background.green(),Background.blue());
                                (*difSon)->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
                                //              difSon->hide();
                                pageAct=*difSon;
                                //				qWarning("DIFSON new.- .- .- .--. ");
                                break;
                            case ANTIINTRUSIONE:
                                *antintr = new antintrusione(NULL,"ANTI");
                                (*antintr)->setBGColor(Background.red(),Background.green(),Background.blue());
                                (*antintr)->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
                                //              automazioni->hide();
                                pageAct=*antintr;
                                //				qWarning("ANTIINTRUSIONE new.- - ..- -.  .- .- .-");
                                break;
                            case TERMOREGOLAZIONE:
                                *termo = new termoregolaz( NULL,"TERMO", 4, MAX_WIDTH, MAX_HEIGHT,1);
                                (*termo)->setBGColor(Background);
                                (*termo)->setFGColor(Foreground);
                                //              termo->hide();
                                pageAct=*termo;
                                //				qWarning("TERMOREGOLAZIONE new.- .- - .. -. -. -. . ");
                                break;
                            case IMPOSTAZIONI:
                                *imposta = new sottoMenu (NULL,"IMPOSTA");
                                (*imposta) ->setBGColor(Background);
                                (*imposta) ->setFGColor(Foreground);
                                QObject::connect(*imposta,SIGNAL(setPwd(bool,char*)), BtM, SLOT (setPwd(bool,char*))); 
                                //              imposta->hide();	      		
                                pageAct=*imposta;
                                //				qWarning("IMPOSTAZIONI new.- .- . -. -.- .- -. .-.");
                                break;
                            case SCHEDULAZIONI:
                                *sched = new sottoMenu (NULL,"IMPOSTA");
                                (*sched) ->setBGColor(Background);
                                (*sched) ->setFGColor(Foreground);
                                pageAct=*sched;
                                //				qWarning("SCHEDULAZIONI new.- .- . -. -.- .- -. .-.");
                                break;
                            case SPECIAL:	    
                                (*specPage) = new homePage(NULL,"SPECIAL",Qt::WType_TopLevel | Qt::WStyle_Maximize | Qt::WRepaintNoErase);
                                //              specPage ->hide();
                                pageAct=*specPage;
                                (*specPage) ->setBGColor(Background.red(),Background.green(),Background.blue());
                                (*specPage) ->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
                                (*specPage) ->addButton(0,260,ICON_FRECCIA_SX ,BACK);				 
                                //				qWarning("SPECIAL new.- .- . -.-  .- .-");
                                break;
                            } // switch (page_id)
                            
                            pageAct->hide();
                            //	  pageAct->setBGColor((int)bg_r, (int)bg_g, (int)bg_b);
                            //	  pageAct->setFGColor((int)fg_r,(int)fg_g,(int)fg_b);
                            if (idPageDefault==page_id)
                                *pagDefault=pageAct;	  
                            
                            
                            
                        } // if (!CurTagL4.compare("id"))
                else if (!CurTagL4.compare("descr"))
                {
                            page_descr = qValue;
                        }
                else if (CurTagL4.startsWith("item"))
                {
                            CurTagL4_copy=CurTagL4;
                            CurTagL4_copy.remove("item");
                            itemNum=CurTagL4_copy.toInt( &ok, 10 );
                            if (!CurTagL5.compare("id"))
                            {
                                page_item_id = qValue.toInt( &ok, 10 );
                                //				qWarning("PAGEITEM:ID %d",page_item_id);
                            } else if (!CurTagL5.compare("descr"))
                                page_item_descr = qValue;
                            else if (!CurTagL5.compare("where"))
                                page_item_where = qValue;
                            else if (!CurTagL5.compare("what"))
                                page_item_what = qValue;	  
                            else if (CurTagL5.startsWith("element"))
                            {
                                    if (!CurTagL6.compare("where"))
                                    {
                                        page_item_list_group->append(new QString(qValue));
                                    }
                                }
                            else if ((CurTagL5.startsWith("cimg"))||(!CurTagL5.compare("value"))||(!CurTagL5.compare("hour"))||(!CurTagL5.compare("minute")))
                            {
                                    //				    qDebug("FOR PAGEITEM:IMG=%s",qValue.ascii());
                                    QString sValue=qValue;
                                    if  (CurTagL5.startsWith("cimg"))
                                        sValue.prepend(IMG_PATH);
                                    page_item_list_img->append(new QString(sValue));
                                }
                            else if (CurTagL5.startsWith("txt"))
                            {
                                    //				    qDebug("FOR PAGEITEM:TXT=%s",qValue.ascii());
                                    page_item_list_txt->append(new QString(qValue));
                                }  
                            else if (!CurTagL5.compare("type"))
                            {
                                    //				    qDebug("FOR PAGEITEM:TYPE=%s",qValue.ascii());
                                    par2=qValue.toInt( &ok, 10 );
                                }   
                            else if (!CurTagL5.compare("alarmset"))
                            {
                                    //				    qDebug("FOR PAGEITEM:ALARMSET=%s",qValue.ascii());
                                    par3=qValue.toInt( &ok, 10 );
                                }   	 
                            else if (!CurTagL5.compare("enabled"))
                            {
                                    //				    qDebug("FOR PAGEITEM:PASSWORD ENABLED=%s",qValue.ascii());
                                    par1=qValue.toInt( &ok, 10 );
                                }   	 
                            /*			    else if (!CurTagL5.compare("value"))
                            {
                                    par2=qValue.toInt( &ok, 10 );
                                }   */
                        } // if (!CurTagL4.startsWith("item"))
                else if (!CurTagL4.compare("command"))
                {
                            if (!CurTagL5.compare("id"))
                            {
                                    page_item_id = qValue.toInt( &ok, 10 );
                                    //				qWarning("PAGEITEM:ID %d",page_item_id);
                                }
                            else if (!CurTagL5.compare("descr"))
                                page_item_descr=qValue;
                            else if (!CurTagL5.compare("type"))
                                page_item_type=qValue;
                            else if (!CurTagL5.compare("who"))   
                                page_item_who=qValue;
                            else if (!CurTagL5.compare("what"))
                                page_item_what=qValue;
                            else if (!CurTagL5.compare("where"))
                                page_item_where=qValue;
                            else if (CurTagL5.startsWith("cimg"))
                            {
                                    //				    qDebug("FOR PAGEITEM:IMG=%s",qValue.ascii());
                                    QString sValue=qValue;
                                    sValue.prepend(IMG_PATH);
                                    page_item_list_img->append(new QString(sValue));
                                }		
                        }   	 
            } // else if (CurTagL4.startsWith("page"))
        } // if (!CurTagL2.startsWith("displaypages"))
        else if (!CurTagL2.compare("setup"))
        {
            if (!CurTagL3.compare("scs"))
            {
                if (!CurTagL4.compare("coordinate_scs"))	
                {
                            if (!CurTagL5.compare("scs_addr"))		
                            {
                                    datiGen->setAddr(qValue.toInt( &ok, 16 ));
                                }
                        }
            }
            else if (!CurTagL3.compare("generale"))
            {
                if (!CurTagL4.compare("clock"))	
                {
                            if (!CurTagL5.compare("dateformat"))		
                            {
                                    tipoData=qValue.toInt( &ok, 10 );//FORMATO DATA
                                }
                        }
            }
        }
    } // if (!CurTagL1.startsWith("configuratore"))
    
    return TRUE;
}
