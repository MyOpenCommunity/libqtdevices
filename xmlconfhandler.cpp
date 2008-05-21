/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **xmlconfhandler.cpp
 **
 **classe per il parsing SAX di conf.xml
 **
 ****************************************************************/

#include <qobject.h>
#include <qtimer.h>
#include <qregexp.h>

#include "xmlconfhandler.h"
#include "main.h"
#include "homepage.h"
#include "sottomenu.h"
#include "impostatime.h"
#include "diffsonora.h"
#include "diffmulti.h"
#include "sveglia.h"
#include "genericfunz.h"
#include "versio.h"
#include "antintrusione.h"
#include "thermalmenu.h"
#include "btmain.h"
#include "scenevocond.h"
#include "openclient.h"

unsigned char tipoData=0;

/*******************************************
 *
 *******************************************/
xmlconfhandler::xmlconfhandler(BtMain *BM, homePage**h, homePage**sP, sottoMenu**se, sottoMenu **vc, sottoMenu *i, sottoMenu**s,
		sottoMenu**c, sottoMenu**im,  sottoMenu**a, ThermalMenu *t, diffSonora**dS, diffmulti**_dm, antintrusione** ant,
		QWidget** pD,Client * c_c, Client *  c_m ,Client *  c_r,versio* dG, QColor* bg, QColor* fg1, QColor *fg2)
{
	home=h;
	specPage=sP;
	scenari_evoluti = se;
	videocitofonia = vc;
	illumino=i;
	scenari=s;
	carichi=c;
	imposta=im;
	automazioni=a;
	termo=t;
	difSon=dS;
	dm=_dm;
	antintr=ant;
	pagDefault=pD;
	BtM=BM;
	client_comandi=c_c;
	client_monitor=c_m;
	client_richieste=c_r;
	datiGen=dG;

	//   bg_r, bg_g, bg_b,fg_r,fg_g, fg_b,fg_r1, fg_g1, fg_b1;
	Background=*bg;
	//qDebug("scello %d",bg);
	Foreground=*fg1;
	//qDebug("scello1 %d",bg);
	SecondForeground=*fg2;//QColor(255,0,0);
	//qDebug("scello2 %d",bg);
	page_item_list_img = new QPtrList<QString>;
	page_item_list_img_m = new QPtrList<QString>;
	page_item_list_group = new QPtrList<QString>;
	page_item_list_group_m = new QPtrList<QString>;
	page_item_list_txt = new QPtrList<QString>;
	page_item_list_txt_times = new QPtrList<QString>;
	page_item_cond = NULL;
	page_item_cond_list = new QPtrList<scenEvo_cond>;
	page_item_descr_m = new QPtrList<QString>;
	page_item_unknown = "";
	page_item_txt1 = "";
	page_item_txt2 = "";
	page_item_txt3 = "";
	//qDebug("scello3 %d",bg);
	// Start counting for wd refresh
	wdtime.start();
}

/*******************************************
 *
 *******************************************/
xmlconfhandler::~xmlconfhandler()
{
	delete page_item_list_img;
	delete page_item_list_img_m;
	delete page_item_list_group;
	delete page_item_list_group_m;
	delete page_item_list_txt;
	delete page_item_list_txt_times;
	delete page_item_cond_list;
	delete page_item_descr_m;
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
	page_item_list_img_m->clear();
	page_item_list_group->clear();
	page_item_list_group_m->clear();
	par1=par2=0;
	page_item_list_img->clear();
	page_item_list_txt->clear();
	page_item_list_txt_times->clear();
	page_item_list_group->clear(); 
	page_item_cond_list->clear();
	page_item_what = "";
	page_item_descr = "";
	page_item_where = "";
	page_item_key = "";
	page_item_light = "";
	page_icon = "";
	page_item_who = "";
	page_item_type = "0";
	page_item_mode = "0";
	page_item_softstart = 25;
	page_item_softstop = 25;
	sstart.clear();
	sstop.clear();
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
	hompage_id=0xff;

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

void *xmlconfhandler::getAddr()
{
	char pip[50];
	pip[0] = 0;
	void *pnt = 0;

	if ( (!page_item_what.isNull()) && (!page_item_what.isEmpty())  )
	{			       
		strcpy(pip, page_item_what.ascii());
		strcat(pip,"*");
		strcat(pip,page_item_where.ascii());
	}
	else
		strcpy(pip, page_item_where.ascii());

	if (page_item_list_group->isEmpty())
	{
		pnt=pip;
	}
	else
	{
		pnt=page_item_list_group;
	}

	return pnt;
}

void *xmlconfhandler::computeAddress()
{
	void *pnt = getAddr();

	if ((char)page_item_id==SET_SVEGLIA) 
	{
		if  (par2==sveglia::DI_SON) 
		{
			// Use old or multichannel sd
			if(*difSon)
				pnt = new contdiff(*difSon, NULL);
			else if(*dm)
				pnt = new contdiff(NULL, *dm);
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

	return pnt;
}

void xmlconfhandler::addItemU(sottoMenu *sm, void *address)
{
	sm->addItemU ((char)page_item_id, page_item_descr,
			address,
			*page_item_list_img,
			par1,  par2, SecondForeground,
			(char*)page_item_list_txt->at(0)->ascii(),
			(char*)page_item_list_txt->at(1)->ascii(),
			(char*)page_item_list_txt->at(2)->ascii(),
			(char*)page_item_list_txt->at(3)->ascii(),
			par3, par4,
			page_item_list_txt_times, page_item_cond_list, page_item_action,
			page_item_light, page_item_key, page_item_unknown, sstart, sstop,
			page_item_txt1, page_item_txt2, page_item_txt3);
	page_item_cond_list->clear();
}

void xmlconfhandler::createSottoMenuConnections(sottoMenu *sm)
{
	QObject::connect(sm,SIGNAL(Closed()),sm,SLOT(hide()));
	QObject::connect(client_monitor,SIGNAL(frameIn(char *)),sm,SIGNAL(gestFrame(char *)));
	QObject::connect(sm,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
	QObject::connect(sm,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
	QObject::connect(sm,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
	QObject::connect(BtM,SIGNAL(freeze(bool)),sm,SLOT(freezed(bool)));
}

/*******************************************
 *
 * Esco da un livello
 *
 *
 *******************************************/
bool xmlconfhandler::endElement( const QString&, const QString&, const QString& )
{
	{
		if(wdtime.elapsed() > 1000) {
			wdtime.restart();
			qDebug("Invoking rearmWDT()");
			rearmWDT();
		}
	}
	
	
	if (!car)
		characters( QString("\000"));        // se ho un tag vuoto riempio con '\000' il suo campo 


	if (!CurTagL1.compare("configuratore"))
	{
		if (!CurTagL2.compare("displaypages"))
		{
			if ( !CurTagL3.compare("homepage") && (CurTagL4.isEmpty()) )
			{
				qWarning("HOMEPAGE");
				qWarning("hompage_isdefined=%d",hompage_isdefined);
				qWarning("hompage_id=%d",hompage_id);	
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
				/** PAGEMENU
				 *  nel file conf.xml c'è homepage, pagemenu e page0, page1,..., pagen
				 *  pagemenu è il menù principale che viene usato anche come homepage se homepage non è definita.
				 *  Ogni pagina è un sottomenu e contiene vari item.
				 */
				if ( CurTagL4.startsWith("item") && CurTagL5.isEmpty() )
				{
					/*
					qWarning("sottomenu_id=%d",sottomenu_id);
					qWarning("sottomenu_descr=%s",(const char*)sottomenu_descr);
					qWarning("sottomenu_left=%d",sottomenu_left);
					qWarning("sottomenu_top=%d",sottomenu_top);
					qWarning("sottomenu_icon_where=%s",(const char*)sottomenu_where);
					qWarning("sottomenu_icon_name=%s",(const char*)sottomenu_icon_name);
					*/
					switch (sottomenu_id)
					{
					case AUTOMAZIONE:
					case ILLUMINAZIONE:
					case ANTIINTRUSIONE:
					case CARICHI:
					case TERMOREGOLAZIONE:
					case DIFSON:
					case DIFSON_MULTI:
					case SCENARI:
					case IMPOSTAZIONI: 				  // addbutton normali
					case SCENARI_EVOLUTI:
					case VIDEOCITOFONIA:
						(*home)->addButton(sottomenu_left,sottomenu_top,(char *)sottomenu_icon_name.ascii(), (char)sottomenu_id);
						break;
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
					case TERMO_HOME_PROBE:
						(*home)->addTemp((char *)sottomenu_where.ascii(),sottomenu_left+10,sottomenu_top+10,220,60,Background,Foreground,QFrame::Plain,3,"");
						//			qWarning("ADDBUTTON TEMPERATURA");
						break;
					case TERMO_HOME_EXTPROBE:
						(*home)->addTemp((char *)sottomenu_where.ascii(),sottomenu_left+10,sottomenu_top+10,220,60,Background,Foreground,QFrame::Plain,3,"", "1");
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
				if ( (( CurTagL4.startsWith("device") || CurTagL4.startsWith("item") || !CurTagL4.compare("command") ) && CurTagL5.isEmpty()))
				{
					qDebug("INSERTED ITEM:ID %d",page_item_id);
					qDebug("INS ITEM: %s",banTesti[page_item_id]);
					qDebug("DESCR: %s", page_item_descr.ascii());

					/*		    for ( QString * MyPnt = page_item_list_img->first(); MyPnt; MyPnt= page_item_list_img->next() )
							    qWarning("IMG=%s su %d",MyPnt->ascii(),page_item_list_img->count());*/

					for(int idx=page_item_list_img->count();idx<11;idx++)
					{
						page_item_list_img->append(new QString(""));
					}
					for(int idx=page_item_list_txt->count();idx<4;idx++)
					{
						page_item_list_txt->append(new QString(""));
					}		     
#if 0
					for(int idx=page_item_list_txt_times->count(); 
							idx < 20; idx++)
					{
						page_item_list_txt_times->append(new QString(""));
					}
#endif
					/*		    for (QString * MyPnt = page_item_list_group->first(); MyPnt; MyPnt = page_item_list_group->next() )
							    qWarning("GROUP=%s",MyPnt->ascii());	*/

					sottoMenu *pageAct = NULL;
					void *addr = 0;
					char pip[50];
					pip[0] = 0;
					void *pnt = 0;
					switch (  page_id )
					{
						case AUTOMAZIONE:
							pageAct = *automazioni;
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case  ILLUMINAZIONE:
							pageAct= illumino; 
							par3 = page_item_softstart; 
							par4 = page_item_softstop;
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case CARICHI:
							pageAct=(*carichi); 		
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case TERMOREGOLAZIONE:
							pageAct=termo;
							addr = computeAddress();
							//addItemU(pageAct, addr);
							break;

						case  SCENARI:
							pageAct=(*scenari); 		
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case SCENARI_EVOLUTI:
							pageAct= (*scenari_evoluti);
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case VIDEOCITOFONIA:
							pageAct = (*videocitofonia);
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case IMPOSTAZIONI:
							pageAct=(*imposta);
							addr = computeAddress();
							addItemU(pageAct, addr);
							break;

						case ANTIINTRUSIONE:
							(*antintr)->addItemU ((char)page_item_id, page_item_descr,
								(char*)page_item_where.ascii(),
								*page_item_list_img, par1,  par2, (char*)page_item_list_txt->at(0)->ascii(),   (char*)page_item_list_txt->at(1)->ascii(),  (char*)page_item_list_txt->at(2)->ascii(),  (char*)page_item_list_txt->at(3)->ascii())  ;
							break;
						case DIFSON:
							if ((!page_item_what.isNull()) &&
									(!page_item_what.isEmpty())) {
								strcpy(pip, page_item_what.ascii());
								strcat(pip,"*");
								strcat(pip,page_item_where.ascii());
							} else
								strcpy(pip, page_item_where.ascii());


							if (page_item_list_group->isEmpty()) {
								pnt=pip;
							} else {
								pnt=page_item_list_group;
							}

							// Use third digit of where for audio player frames
							// FIXME aleph: better place for this
							par2 = pip[2] - '0';

							par1 = page_item_mode.toInt();
							(*difSon)->addItemU ((char)page_item_id, page_item_descr, pnt,
								*page_item_list_img,
								par1,  par2);
							break;

						case DIFSON_MULTI:
							if((page_item_id == SORG_RADIO)                      ||
									(page_item_id==SORG_AUX)             ||
									(page_item_id==AMBIENTE)             ||
									(page_item_id==INSIEME_AMBIENTI)     ||
									(page_item_id==SORGENTE_MULTIM)      ||
									(page_item_id==SORGENTE_MULTIM_MC))
							{
								qDebug("clearing descr list");
								page_item_descr_m->clear();
								qDebug("appending %s to descr list", page_item_descr.ascii());
								page_item_descr_m->append(new QString(page_item_descr));
							}
							
							par2 = atoi(page_item_where.ascii());

							(*dm)->addItem ((char)page_item_id, page_item_descr_m, pip,
								*page_item_list_img, par1, par2, QColor(0,0,0));
							qDebug("clearing descr list");
							page_item_descr_m->clear();
							break;

						case SPECIAL:
							qDebug("special");
							switch(page_item_id)
							{
								case TEMPERATURA:
								case TERMO_HOME_PROBE:
									(*specPage) ->addTemp((char*)page_item_where.ascii(),10,(itemNum-1)*80+10,220,60,Background,Foreground,(int)QFrame::Plain,3,(char*)page_item_descr.ascii());
									break;
								case TERMO_HOME_EXTPROBE:
									(*specPage) ->addTemp((char*)page_item_where.ascii(),10,(itemNum-1)*80+10,220,60,Background,Foreground,(int)QFrame::Plain,3,(char*)page_item_descr.ascii(), "1");
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
									(*specPage) ->addDescrU(page_item_descr, 60,240,180,20,Background,Foreground,QFrame::Plain,3);
									pageAct=NULL;
									break;
							}
						default:
							qDebug("xmlconfhandler: sottoMenu type unknown!!");
					}

					set_page_item_defaults();
				} // if ( CurTagL4.startsWith("item") && CurTagL5.isEmpty() )
				else if (CurTagL4.startsWith("item") && 
						CurTagL5.startsWith("device") && 
						CurTagL6.isEmpty()) {
					char pip[50];
					memset(&pip[0],'\000',sizeof(pip));
					void* pnt;
					qDebug("DIFF SON MULTI END ELEMENT !!!!");
					qDebug("INSERTED ITEM:ID %d",page_item_id_m);
					qDebug("INS ITEM: %s",banTesti[page_item_id_m]);
					qDebug("DESCR = %s", 
							(page_item_descr_m->at(0))->ascii());
					strcpy(&pip[0], page_item_where_m.ascii());

					if (page_item_list_group_m->isEmpty()) {
						pnt=pip;
					} else {
						qDebug("**** DIFSON_MULTI: multi address");
						pnt=page_item_list_group_m;
					}
					(*dm)->addItem((char)page_item_id_m, 
						page_item_descr_m,
						pnt, 
						*page_item_list_img_m);
					qDebug("clearing descr list !!!\n");
					page_item_descr_m->clear();
					page_item_list_img_m->clear();

				}
				else if ( CurTagL3.startsWith("page") && CurTagL4.isEmpty() )
				{
					// Esco dalla pagina
					//		    qWarning("DRAW %s",(const char*)page_descr);


					switch (page_id)
					{
						case AUTOMAZIONE:
							//			qWarning(".- .- .- -. -.  .- -.QObject::connect AUTOMAZIONE");
							(*automazioni)->forceDraw();
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
							QObject::connect(*automazioni,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*automazioni,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*automazioni,SLOT(freezed(bool)));
							//(*automazioni)->inizializza();
							break;
						case ILLUMINAZIONE:
							//			qWarning("-. .- . -. - -. .-. -QObject::connect ILLUMINAZIONE");
							illumino->forceDraw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                       
							QObject::connect(*home,SIGNAL(Illuminazione()),illumino,SLOT(showFullScreen()));
							QObject::connect(illumino,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)       
							QObject::connect(*home,SIGNAL(Illuminazione()),illumino,SLOT(show()));
							QObject::connect(illumino,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                          
							QObject::connect(illumino,SIGNAL(Closed()),illumino,SLOT(hide()));			
							QObject::connect(client_monitor,SIGNAL(frameIn(char *)),illumino,SIGNAL(gestFrame(char *)));
							QObject::connect(illumino,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
							QObject::connect(illumino,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(illumino,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(illumino,SIGNAL(richStato(char *)),client_richieste,SLOT(richStato(char *)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),illumino,SLOT(freezed(bool)));
							//(illumino)->inizializza();
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
							QObject::connect(*antintr,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*antintr,SIGNAL(sendFramew(char*)),client_comandi,SLOT(ApriInviaFrameChiudiw(char*)));
							QObject::connect(client_comandi,SIGNAL(openAckRx()),*antintr,SIGNAL(openAckRx()));
							QObject::connect(client_comandi,SIGNAL(openNakRx()),*antintr,SIGNAL(openNakRx()));
							QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*antintr,SLOT(gesFrame(char *)));    
							QObject::connect(*antintr,SIGNAL(Closed()),*antintr,SLOT(hide()));

							QObject::connect(*antintr,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*antintr,SIGNAL(freezed(bool)));
							//(*antintr)->inizializza();
							break;
						case CARICHI:
							//			qWarning(".- .- .- .- .-  -. .- .- .- QObject::connect CARICHI");
							(*carichi)->forceDraw();
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
							QObject::connect(*carichi,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*carichi,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*carichi,SLOT(freezed(bool)));
							break;
						case TERMOREGOLAZIONE:
						case TERMOREG_MULTI_PLANT:
							termo->forceDraw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
							QObject::connect(*home,SIGNAL(Termoregolazione()),termo,SLOT(showFullScreen()));
							QObject::connect(termo,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)    
							QObject::connect(*home,SIGNAL(Termoregolazione()),termo,SLOT(show()));
							QObject::connect(termo,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                    
							createSottoMenuConnections(termo);
							break;
						case DIFSON:
							//	qWarning("- - -. .-  .- .- .- .- .- QObject::connect DIFSON");
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
							QObject::connect(*difSon,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*difSon,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*difSon,SLOT(freezed_handler(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*difSon,SIGNAL(freezed(bool)));
							//(*difSon)->inizializza();
							break;
						case DIFSON_MULTI:
							//	qWarning("- - -. .-  .- .- .- .- .- QObject::connect DIFSON");
							//(*dm)->draw(); 
							(*dm)->forceDraw();
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                       
							QObject::connect(*home,SIGNAL(Difmulti()),*dm,SLOT(showFullScreen()));
							QObject::connect(*dm,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)     
							QObject::connect(*home,SIGNAL(Difmulti()),*dm,SLOT(show()));
							QObject::connect(*dm,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                      
							QObject::connect(*dm,SIGNAL(Closed()),*dm,SLOT(hide()));
							QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*dm,SLOT(gestFrame(char *)));
							QObject::connect(*dm,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
							QObject::connect(*dm,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*dm,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*dm,SIGNAL(freezed(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*dm,SLOT(freezed_handler(bool)));
							//(*difSon)->inizializza();
							break;
						case SCENARI:
							//			qWarning("- - -  - - - - - - - -QObject::connect ");
							(*scenari)->forceDraw();
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
							QObject::connect(*scenari,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*scenari,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*scenari,SLOT(freezed(bool)));
							QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*scenari,SIGNAL(gestFrame(char *)));
							//(*scenari)->inizializza();
							break;
						case IMPOSTAZIONI:    
							//    qWarning("- - - - - - - - - - - QObject::connect IMPOSTAZIONI");
							(*imposta)->forceDraw();
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
							QObject::connect(*imposta,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(*imposta,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(*imposta,SIGNAL(freeze(bool)),BtM,SLOT(freezed(bool)));
							QObject::connect(*imposta,SIGNAL(svegl(bool)),BtM,SLOT(svegl(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*imposta,SLOT(freezed(bool)));

							QObject::connect(*imposta,SIGNAL(startCalib()),
									BtM, SLOT(startCalib()));
							QObject::connect(*imposta,SIGNAL(endCalib()),
									BtM, SLOT(endCalib()));
							break;
						case SCENARI_EVOLUTI:
							qDebug("******* scenari_evoluti = %p, "
									"impostazioni = %p ******", *scenari_evoluti,
									*imposta);
							(*scenari_evoluti)->forceDraw();
							QObject::connect(*scenari_evoluti,SIGNAL(sendFrame(char *)), client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
							QObject::connect(*scenari_evoluti,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*scenari_evoluti,SIGNAL(gestFrame(char *)));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
							QObject::connect(*home,SIGNAL(ScenariEvoluti()),*scenari_evoluti,SLOT(showFullScreen()));
							QObject::connect(*scenari_evoluti,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)      
							QObject::connect(*home,SIGNAL(ScenariEvoluti()),*scenari_evoluti,SLOT(show()));
							QObject::connect(*scenari_evoluti,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                    
							QObject::connect(*scenari_evoluti,SIGNAL(Closed()),*scenari_evoluti,SLOT(hide()));
							QObject::connect(*scenari_evoluti,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(*scenari_evoluti,SIGNAL(freeze(bool)),BtM,SLOT(freezed(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*scenari_evoluti,SLOT(freezed(bool)));
							break;
						case VIDEOCITOFONIA:
							qDebug("******* videocitofonia = %p ", *videocitofonia);
							(*videocitofonia)->forceDraw();
							QObject::connect(*videocitofonia,SIGNAL(sendFrame(char *)), client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
							QObject::connect(*videocitofonia,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
							QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*videocitofonia,SIGNAL(gestFrame(char *)));
#if defined (BTWEB) ||  defined (BT_EMBEDDED)                            
							QObject::connect(*home,SIGNAL(Videocitofonia()),*videocitofonia,SLOT(showFullScreen()));
							QObject::connect(*videocitofonia,SIGNAL(Closed()),*home,SLOT(showFullScreen()));
#endif                                          
#if !defined (BTWEB) && !defined (BT_EMBEDDED)      
							QObject::connect(*home,SIGNAL(Videocitofonia()),*videocitofonia,SLOT(show()));
							QObject::connect(*videocitofonia,SIGNAL(Closed()),*home,SLOT(show()));
#endif                                    
							QObject::connect(*videocitofonia,SIGNAL(Closed()),*videocitofonia,SLOT(hide()));
							QObject::connect(*videocitofonia,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));
							QObject::connect(*videocitofonia,SIGNAL(freeze(bool)),BtM,SLOT(freezed(bool)));
							QObject::connect(*videocitofonia,SIGNAL(svegl(bool)),BtM,SLOT(svegl(bool)));
							QObject::connect(BtM,SIGNAL(freeze(bool)),*videocitofonia,SLOT(freezed(bool)));
							break;
						case SPECIAL:    
							//	qWarning("-- - - - - - -- - - QObject::connect SPECIAL");
							//(*specPage)->draw();
							QObject::connect(BtM,SIGNAL(freeze(bool)),*specPage,SLOT(freezed(bool)));          
							QObject::connect(*specPage,SIGNAL(freeze(bool)),BtM,SIGNAL(freeze(bool)));     
							QObject::connect(*specPage,SIGNAL(sendFrame(char *)),client_comandi,SLOT(ApriInviaFrameChiudi(char *)));
							QObject::connect(*specPage,SIGNAL(sendInit(char *)),client_richieste,SLOT(ApriInviaFrameChiudi(char *)));  
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
				} else if(CurTagL3.startsWith("page") && 
						CurTagL4.startsWith("item") && 
						CurTagL5.startsWith("cond") && 
						CurTagL6.isEmpty()) {
					if(page_item_cond) {
						if(!(page_item_cond->getVal())) {
							// Value is 0, condition has to be deleted
							delete page_item_cond;
						} else {
							page_item_cond->SetIcons();
							page_item_cond_list->append(page_item_cond);
						}
						page_item_cond = NULL;
					}
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

QDomNode getThermRootNode()
{
	QDomElement root = qdom_appconfig.documentElement();

	QDomNode n = root.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName() == "displaypages")
			break;
		n = n.nextSibling();
	}

	n = n.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(QRegExp("page\\d\\d?")))
		{
			QDomNode child = n.firstChild();
			while (child.nodeName() != "id")
			{
				child = child.nextSibling();
			}
			
			QDomElement e = child.toElement();
			if (e.text() == "5" || e.text() == "15")
			{
				return n;
			}
		}
		n = n.nextSibling();
	}
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
				// qDebug(qValue.ascii());
				setOrientation((char*)qValue.ascii()); 	
			}

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
					if((!hompage_id) && (hompage_isdefined))
						*pagDefault=*home;
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
						qDebug ("PAGEMENU:icon_name %s",sottomenu_icon_name.ascii());
					} else if (!CurTagL5.compare("where"))
						sottomenu_where = qValue;

				} // if (!CurTagL4.startsWith("item"))
			} // if (!CurTagL3.startsWith("pagemenu"))
			/**
			// Legge le varie pagine: page1, page2,..., pagen
			*/
			else if (CurTagL3.startsWith("page"))
			{
				if ((CurTagL3 == "pagevct") && (CurTagL4 == "unknown"))
					page_item_unknown = qValue;

				if ((CurTagL3 == "pagevct") && (CurTagL4.startsWith("txt")))
				{
					if (page_item_txt1 == "")
						page_item_txt1 = qValue;
					else if (page_item_txt2 == "")
						page_item_txt2 = qValue;
					else
						page_item_txt3 = qValue;
				}
				if (CurTagL4 == "id")
				{
					QWidget* pageAct=NULL;
					page_id = qValue.toInt( &ok, 10 );
					qDebug("INSERTING PAGE: %s",pagTesti[page_id-1]);
					QDomNode n;


					switch (page_id)
					{
						case AUTOMAZIONE:
							*automazioni = new sottoMenu (NULL,"AUTOM");
							(*automazioni)->setBGColor(Background);
							(*automazioni)->setFGColor(Foreground);
							pageAct=*automazioni;
							break;

						case ILLUMINAZIONE:
							illumino = new sottoMenu (NULL,"ILLUMINO");
							illumino->setBGColor(Background);
							illumino->setFGColor(Foreground);
							pageAct=illumino;
							break;

						case DIFSON_MULTI:
							*dm = new diffmulti(NULL, "DIFSON_MULTI");
							(*dm)->setBGColor(Background);
							(*dm)->setFGColor(Foreground);
							pageAct = *dm;
							page_item_id_m = 0;
							page_item_descr_m->clear();
							page_item_where_m = "";
							page_item_list_img_m->clear();
							break;

						case SCENARI:
							*scenari = new sottoMenu (NULL,"SCENARI");
							(*scenari)->setBGColor(Background);
							(*scenari)->setFGColor(Foreground);
							pageAct=*scenari;
							break;

						case CARICHI:
							*carichi = new sottoMenu (NULL,"CARICHI");
							(*carichi)->setBGColor(Background);
							(*carichi)->setFGColor(Foreground);
							pageAct=*carichi;
							break;

						case DIFSON:
							*difSon = new diffSonora (NULL,"DIFSON");
							(*difSon)->setBGColor(Background.red(),Background.green(),Background.blue());
							(*difSon)->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
							pageAct=*difSon;
							break;

						case ANTIINTRUSIONE:
							*antintr = new antintrusione(NULL,"ANTI");
							(*antintr)->setBGColor(Background.red(),Background.green(),Background.blue());
							(*antintr)->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
							pageAct=*antintr;
							break;

						case TERMOREGOLAZIONE:
							n = getThermRootNode();
							termo = new ThermalMenu(NULL, "TERMO", n, Background, Foreground);
							pageAct=termo;
							break;

						case TERMOREG_MULTI_PLANT:
							n = getThermRootNode();
							termo = new ThermalMenu(NULL, "TERMO", n, Background, Foreground);
							pageAct=termo;
							break;

						case IMPOSTAZIONI:
							*imposta = new sottoMenu (NULL,"IMPOSTA");
							(*imposta) ->setBGColor(Background);
							(*imposta) ->setFGColor(Foreground);
							QObject::connect(*imposta,SIGNAL(setPwd(bool,char*)), BtM, SLOT (setPwd(bool,char*)));
							pageAct=*imposta;
							break;

						case SCENARI_EVOLUTI:
							*scenari_evoluti = new sottoMenu (NULL,"SCENARI_EVOLUTI");
							(*scenari_evoluti) ->setBGColor(Background);
							(*scenari_evoluti) ->setFGColor(Foreground);
							pageAct=*scenari_evoluti;
							break;

						case VIDEOCITOFONIA:
							*videocitofonia = new sottoMenu (NULL, "VIDEOCITOFONIA");
							(*videocitofonia)->setBGColor(Background);
							(*videocitofonia)->setFGColor(Foreground);
							pageAct=*videocitofonia;
							break;

						case SPECIAL:
							qDebug("!");
							(*specPage) = new homePage(NULL,"SPECIAL",Qt::WType_TopLevel | Qt::WStyle_Maximize | Qt::WRepaintNoErase);
							pageAct=*specPage;
							(*specPage) ->setBGColor(Background.red(),Background.green(),Background.blue());
							(*specPage) ->setFGColor(Foreground.red(),Foreground.green(),Foreground.blue());
							(*specPage) ->addButton(0,260,ICON_FRECCIA_SX ,BACK);
							break;
					} // switch (page_id)
					pageAct->hide();
					if ( (idPageDefault==page_id) && (hompage_isdefined) )
						*pagDefault=pageAct;

				} // if (!CurTagL4.compare("id"))
				else if (!CurTagL4.compare("descr"))
				{
					page_descr = qValue;
				}
				else if ((CurTagL4.startsWith("item") || CurTagL4.startsWith("device")) && (!CurTagL5.startsWith("device")))
				{
					CurTagL4_copy=CurTagL4;
					if(CurTagL4.startsWith("item"))
						CurTagL4_copy.remove("item");
					else
						CurTagL4_copy.remove("device");
					itemNum=CurTagL4_copy.toInt( &ok, 10 );
					if (!CurTagL5.compare("id"))
					{
						page_item_id = qValue.toInt( &ok, 10 );
						//				qWarning("PAGEITEM:ID %d",page_item_id);
					} else if (!CurTagL5.compare("descr"))
						page_item_descr = qValue;
					else if (!CurTagL5.compare("mode"))
						page_item_mode = qValue;
					else if (!CurTagL5.compare("softstart"))
						page_item_softstart = qValue.toInt( &ok, 10);
					else if (!CurTagL5.compare("softstop"))
						page_item_softstop = qValue.toInt( &ok, 10);
					else if (CurTagL5.startsWith("time"))
						page_item_list_txt_times->append(new QString(qValue));
					else if (!CurTagL5.compare("where"))
						page_item_where = qValue;
					else if (!CurTagL5.compare("what"))
						page_item_what = qValue;
					else if (!CurTagL5.compare("light"))
						page_item_light = qValue;
					else if (!CurTagL5.compare("key"))
						page_item_key = qValue;
					else if (CurTagL5.startsWith("element"))
					{
						if (!CurTagL6.compare("where"))
						{
							page_item_list_group->append(new QString(qValue));
						}
						if (!CurTagL6.compare("softstart")) {
							qDebug("**** QUA %d", qValue.toInt());
							sstart.append(qValue.toInt());
						}
						if (!CurTagL6.compare("softstop")) {
							qDebug("**** QUA %d", qValue.toInt());
							sstop.append(qValue.toInt());
						}
					}
					else if ((CurTagL5.startsWith("cimg"))||(!CurTagL5.compare("value"))||(!CurTagL5.compare("hour"))||(!CurTagL5.compare("minute")))
					{
						qDebug("FOR PAGEITEM:IMG=%s",qValue.ascii());
						QString sValue=qValue;
						if  (CurTagL5.startsWith("cimg"))
							sValue.prepend(IMG_PATH);
						qDebug("cimg %s", sValue.ascii());
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
					else if(!CurTagL5.compare("enable"))
					{
						par3 = qValue.toInt(&ok, 10);
					}
					else if(!CurTagL5.compare("condH"))
					{
						if(!page_item_cond)
						{
							page_item_cond = new scenEvo_cond_h(NULL,
									"cond h s.evo.");
							QObject::connect(BtM, SIGNAL(resettimer()), page_item_cond, SLOT(setupTimer()));
						}
						scenEvo_cond_h *ch = (scenEvo_cond_h *)
							page_item_cond;
						// Hour condition
						if(!CurTagL6.compare("value")) {
							qDebug("condH, value = %s",
									qValue.ascii());
							ch->setVal(atoi(qValue.ascii()));

						} else if(!CurTagL6.compare("hour")) {
							qDebug("condH, hour = %s", qValue.ascii());
							ch->set_h(qValue.ascii());
						} else if(!CurTagL6.compare("minute")) {
							qDebug("condH, minute = %s",
									qValue.ascii());
							ch->set_m(qValue.ascii());
						} else if(!CurTagL6.compare("cimg1")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								ch->setImg(0, tmp);
							}
						} else if(!CurTagL6.compare("cimg2")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								ch->setImg(1, tmp.ascii());
							}
						} else if(!CurTagL6.compare("cimg3")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								ch->setImg(2, tmp.ascii());
							}
						} else if(!CurTagL6.compare("cimg4")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								ch->setImg(3, tmp.ascii());
							}
						}
					}
					else if(!CurTagL5.compare("condDevice"))
					{
						// Device condition
						if(!page_item_cond)
							page_item_cond = new scenEvo_cond_d(NULL,
									"scen. evo. c.disp.");
						scenEvo_cond_d *cd = (scenEvo_cond_d *)
							page_item_cond;
						if(!CurTagL6.compare("value")) {
							int v = qValue.toInt();
							cd->setVal(v);
						} else if(!CurTagL6.compare("descr")) {
							cd->set_descr(qValue.ascii());
						} else if(!CurTagL6.compare("where")) {
							cd->set_where(qValue.ascii());
						} else if(!CurTagL6.compare("trigger")){
							cd->set_trigger(qValue.ascii());
						} else if(!CurTagL6.compare("cimg1")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								cd->setImg(0, tmp.ascii());
							}
						} else if(!CurTagL6.compare("cimg2")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								cd->setImg(1, tmp.ascii());
							}
						} else if(!CurTagL6.compare("cimg3")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								cd->setImg(2, tmp.ascii());
							}
						} else if(!CurTagL6.compare("cimg4")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								cd->setImg(3, tmp.ascii());
							}
						} else if(!CurTagL6.compare("cimg5")) {
							if(qValue != "") {
								QString tmp(IMG_PATH);
								tmp.append(qValue.ascii());
								cd->setImg(4, tmp.ascii());
							}
						} 
					} else if(!CurTagL5.compare("action")) {
						if(!CurTagL6.compare("open")) {
							page_item_action = qValue;
							qDebug("action = %s", qValue.ascii());
						}
					} else if(!CurTagL5.compare("unable")) {
						if(!CurTagL6.compare("value")) {
							if(!qValue.compare("0")) {
								par1 = 0;
								QString *s = new QString("");
								page_item_list_txt->insert(0, s);
								s = new QString("");
								page_item_list_img->insert(0, s);
							} else
								par1 = 1;
						} else if(!CurTagL6.compare("open") && par1) {
							QString *s = new QString(qValue.ascii());
							page_item_list_txt->insert(0, s);
						} else if(CurTagL6.startsWith("cimg") && par1){
							QString *s = new QString(IMG_PATH);
							s->append(qValue);
							page_item_list_img->insert(0, s);
						}
					} else if(!CurTagL5.compare("disable")) {
						if(!CurTagL6.compare("value")) {
							if(!qValue.compare("0")) {
								QString *s = new QString("");
								page_item_list_txt->insert(1, s);
								s = new QString("");
								page_item_list_img->insert(1, s);
								par1 = 0;
							} else
								par1 = 1;
						} else if(!CurTagL6.compare("open") && par1) {
							QString *s = new QString(qValue.ascii());
							page_item_list_txt->insert(1, s);
						} else if(CurTagL6.startsWith("cimg") && par1){
							QString *s = new QString(IMG_PATH);
							s->append(qValue);
							page_item_list_img->insert(1, s);
						}
					} else if(!CurTagL5.compare("start")) {
						if(!CurTagL6.compare("value")) {
							if(!qValue.compare("0")) {
								QString *s = new QString("");
								page_item_list_txt->insert(2, s);
								s = new QString("");
								page_item_list_img->insert(2, s);
								par1 = 0;
							} else
								par1 = 1;
						} else if(!CurTagL6.compare("open") && par1) {
							QString *s = new QString(qValue.ascii());
							page_item_list_txt->insert(2, s);
						} else if(CurTagL6.startsWith("cimg") && par1){
							QString *s = new QString(IMG_PATH);
							s->append(qValue);
							page_item_list_img->insert(2, s);
						}
					} else if(!CurTagL5.compare("stop")) {
						if(!CurTagL6.compare("value")) {
							if(!qValue.compare("0")) {
								QString *s = new QString("");
								page_item_list_txt->insert(3, s);
								s = new QString("");
								page_item_list_img->insert(3, s);
								par1 = 0;
							} else
								par1 = 1;
						} else if(!CurTagL6.compare("open") && par1) {
							QString *s = new QString(qValue.ascii());
							page_item_list_txt->insert(3, s);
						} else if(CurTagL6.startsWith("cimg") && par1){
							QString *s = new QString(IMG_PATH);
							s->append(qValue);
							page_item_list_img->insert(3, s);
						}
					}
				} // if (CurTagL4.startsWith("item"))
				else if (CurTagL4.startsWith("item") && 
						CurTagL5.startsWith("device")) {
					// Multichannel sound system
					//qDebug("MULTI CHANNEL START ELEMENT");
					CurTagL5_copy=CurTagL5;
					CurTagL5_copy.remove("device");
					itemNum=CurTagL5_copy.toInt( &ok, 10 );
					if (!CurTagL6.compare("id")) {
						page_item_id_m = qValue.toInt( &ok, 10 );
						qWarning("PAGEITEM:ID %d",page_item_id_m);
					} else if (CurTagL6.startsWith("descr")) {
						page_item_descr_m->append(new QString(qValue));
						qDebug("appending to descr list: ");
						qDebug("DESCR = %s", qValue.ascii());
					} else if (CurTagL6.startsWith("where")) {
						if(!CurTagL6.compare("where")) {
							page_item_list_group_m->clear();
							page_item_where_m = qValue;
							qDebug("WHERE = %s", qValue.ascii());
						}  else {
							page_item_list_group_m->append(new QString(qValue));
							qDebug("WHERE_MULTI %s", qValue.ascii());
						}
					} else if(CurTagL6.startsWith("cimg")) {
						QString sValue=qValue;
						sValue.prepend(IMG_PATH);
						qDebug("cimg %s", sValue.ascii());
						page_item_list_img_m->append(new QString(sValue));
					}
				} // CurTagL4.startsWith("item") && CurTagL5.startsWith("device")
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
				}  //else if (!CurTagL4.compare("command"))
				else if (!CurTagL4.compare("type"))
				{
					par3 = qValue.toInt( &ok, 10 );
				}
				else if (!CurTagL4.compare("ind_centrale"))
				{
					// Name doesn't matter, the variable is used as a trick to pass value to termo
					// constructor.
					page_item_txt1 = qValue;
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
				if (!CurTagL4.compare("modello"))
					datiGen->setModelU( qValue );
			}
		}
	} // if (!CurTagL1.startsWith("configuratore"))


	return TRUE;
}
