/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** items.cpp
 **
 **f
 **definizione dei vari items
 ****************************************************************/
#include "items.h"
#include "openclient.h"
#include "sottomenu.h"
#include "device.h"
#include "frame_interpreter.h"
#include "device_cache.h"

//#include "btbutton.h"
#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <stdlib.h>


/*****************************************************************
 **dimmer
 ****************************************************************/

dimmer::dimmer( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon,char* breakIcon, bool to_be_connect )
: bannRegolaz( parent, name )
{
	//setRange(10,90);
	setRange(20, 100);
	setStep(10);
	SetIcons( IconaSx,IconaDx,icon, inactiveIcon,breakIcon,(char)0 );
	setAddress(indirizzo);
	/*      impostaAttivo(1);
		setValue(5);*/
	connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
	if(to_be_connect)
	{
		// Crea o preleva il dispositivo dalla cache
		dev = btouch_device_cache.get_dimmer(getAddress());
		// Get status changed events back
		connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
				this, SLOT(status_changed(QPtrList<device_status>)));
	}
}

void dimmer::Draw()
{
	if(getValue() > 100)
		setValue(100);
	qDebug("dimmer::Draw(), attivo = %d, value = %d", attivo, getValue());
	if ( (sxButton) && (Icon[0]) )
	{
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}

	if ( (dxButton) && (Icon[1]) )
	{
		dxButton->setPixmap(*Icon[1]);
		if (pressIcon[1])
			dxButton->setPressedPixmap(*pressIcon[1]);
	}
	if (attivo==1)
	{
		if ( (Icon[4+((getValue()-minValue)/step)*2]) && (csxButton) )
		{
			csxButton->setPixmap(*Icon[4+((getValue()-minValue)/step)*2]);
			qDebug("* Icon[%d]", 4+((getValue()-minValue)/step)*2);
		}
		if ( (cdxButton) && (Icon[5+((getValue()-minValue)/step)*2]) )
		{
			cdxButton->setPixmap(*Icon[5+((getValue()-minValue)/step)*2]);
			qDebug("** Icon[%d]", 5+((getValue()-minValue)/step)*2);
		}
	}
	else if (attivo==0)
	{
		if ( (Icon[2]) && (csxButton) )
		{
			csxButton->setPixmap(*Icon[2]);
			qDebug("*** Icon[%d]", 2);
		}
		if ( (cdxButton) && (Icon[3]) )
		{
			cdxButton->setPixmap(*Icon[3]);
			qDebug("**** Icon[%d]", 3);
		}
	}
	else if (attivo==2)
	{
		if ( (Icon[44]) && (csxButton) )
		{
			csxButton->setPixmap(*Icon[44]);		    
			qDebug("******* Icon[%d]", 44);
		}

		if ( (cdxButton) && (Icon[45]) )
		{
			cdxButton->setPixmap(*Icon[45]);    
			qDebug("******* Icon[%d]", 45);
		}
	}
	if (BannerText)
	{
		BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
		BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
		BannerText->setText(testo);
		//     qDebug("TESTO: %s", testo);
	}
	if (SecondaryText)
	{	
		SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
		SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
		SecondaryText->setText(testoSecondario);
	}
}

void dimmer::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_speed(stat_var::SPEED);
	stat_var curr_status(stat_var::ON_OFF);
	stat_var curr_fault(stat_var::FAULT);
	int val10;
	bool aggiorna = false;
	qDebug("dimmer10::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::LIGHTS:
				qDebug("Light status variation");
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				impostaAttivo(curr_status.get_val() != 0);
				if(!curr_status.get_val()) {
					// Update 
					aggiorna = true;
					impostaAttivo(0);
				} else
					impostaAttivo(1);
				break;
			case device_status::DIMMER:
				ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
				ds->read(device_status_dimmer::FAULT_INDEX, curr_fault);
				if(curr_fault.get_val()) {
					qDebug("DIMMER FAULT !!");
					impostaAttivo(2);
				} else {
					qDebug("dimmer status variation");
					qDebug("level = %d", curr_lev.get_val());
					setValue(curr_lev.get_val());
					if((curr_lev.get_val() ==0))
						impostaAttivo(0);
				}
				aggiorna = true;
				break;
			case device_status::DIMMER100:
				ds->read(device_status_dimmer100::LEV_INDEX, curr_lev);
				ds->read(device_status_dimmer100::SPEED_INDEX, curr_speed);
				ds->read(device_status_dimmer::FAULT_INDEX, curr_fault);
				qDebug("dimmer 100 status variation, ignored");
#if 0
				qDebug("level = %d, speed = %d", curr_lev.get_val(), 
						curr_speed.get_val());
				val10 = curr_lev.get_val()/10;
				if((curr_lev.get_val() % 10) >= 5)
					val10++;
				val10 *= 10;
				setValue(val10);
				aggiorna = true ;
#endif
				break;
			case device_status::NEWTIMED:
				qDebug("new timed device status variation, ignored");
				//setValue(1);
				//aggiorna = true;
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}

void dimmer:: Accendi()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "1",getAddress(),"");
	emit sendFrame(msg_open.frame_open);   
}
void dimmer:: Spegni()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "0",getAddress(),"");
	emit sendFrame(msg_open.frame_open);   
}
void dimmer:: Aumenta()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "30",getAddress(),"");
	emit sendFrame(msg_open.frame_open);   
}
void dimmer:: Diminuisci()	
{
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "31",getAddress(),"");
	emit sendFrame(msg_open.frame_open);   
}

void dimmer::inizializza(bool forza)
{   
	openwebnet msg_open;
	char    pippo[50];
	qDebug("dimmer::inizializza");

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	if(!forza)
		emit richStato(msg_open.frame_open);    
	else
		emit sendInit(msg_open.frame_open);   
}


/*****************************************************************
 **dimmer 100 livelli
 ****************************************************************/

dimmer100::dimmer100( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon,char* breakIcon,
		int sstart, int sstop)
: dimmer( parent, name, indirizzo, IconaSx, IconaDx, icon, 
		inactiveIcon, breakIcon, false)
{ 
	qDebug("costruttore dimmer100, name = %s", name);
	softstart = sstart;
	qDebug("softstart = %d", softstart);
	softstop = sstop;
	qDebug("softstop = %d", softstop);
	setRange(5,100);
	setStep(5);
	setValue(0);
	qDebug("IconaSx = %s", IconaSx);
	qDebug("IconaDx = %s", IconaDx);
	qDebug("icon = %s", icon);
	qDebug("inactiveIcon = %s", inactiveIcon);
	qDebug("breakIcon = %s", breakIcon);
	SetIcons( IconaSx,IconaDx,icon, inactiveIcon,breakIcon,(char)0 );
	dev = btouch_device_cache.get_dimmer100(getAddress());
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)),
			this, SLOT(status_changed(QPtrList<device_status>)));

}


bool dimmer100::decCLV(openwebnet& msg, char& code, char& lev, char& speed,
		char& h, char &m, char &s)
{
	// Message is a new one if it has the form:
	// *#1*where*1*lev*speed##
	// which is a measure frame
	bool out = msg.IsMeasureFrame();
	if(!out) return out;
	code = atoi(msg.Extract_grandezza());
	qDebug("dimmer100::decCLV, code = %d", code);
	if(code == 2) {
		h = atoi(msg.Extract_valori(0));
		m = atoi(msg.Extract_valori(1));
		s = atoi(msg.Extract_valori(2));
	} else if(code == 1) {
		lev = atoi(msg.Extract_valori(0)) - 100;
		speed = atoi(msg.Extract_valori(1));
	}
	return true;
}

void dimmer100:: Accendi()
{
	qDebug("dimmer100::Accendi()");
	if(isActive()) return;
	//*#1*where*#1*lev*speed
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char s[100];
#if 0
	sprintf(s, "*#1*%s*#1*%d*%d##", getAddress(), last_on_lev, softstart);
#else
	//*1*0#velocita*dove## 
	sprintf(s, "*1*1#%d*%s##", softstart, getAddress());
#endif
	msg_open.CreateMsgOpen(s, strlen(s));
	emit sendFrame(msg_open.frame_open);   
}

void dimmer100:: Spegni()
{
	qDebug("dimmer100::Spegni()");
	if(!isActive()) return;
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();
	char s[100];
	last_on_lev = getValue();
	//*1*0#velocita*dove## 
	sprintf(s, "*1*0#%d*%s##", softstop, getAddress());
	msg_open.CreateMsgOpen(s, strlen(s));
	emit sendFrame(msg_open.frame_open);   
}

void dimmer100:: Aumenta()
{
	qDebug("dimmer100::Aumenta()");
	//if(!isActive()) return;
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();     
	//msg_open.CreateMsgOpen("1", "30",getAddress(),"");
	char cosa[100];
	// Simone agresta il 4/4/2006
	// per l'incremento e il decremento prova ad usare il valore di velocit? di
	// default 255.
	sprintf(cosa, "30#5#255");
	msg_open.CreateMsgOpen("1", cosa, getAddress(), "");
	emit sendFrame(msg_open.frame_open);   
}

void dimmer100:: Diminuisci()	
{
	qDebug("dimmer100::Diminuisci()");
	//if(!isActive()) return;
	openwebnet msg_open;
	char cosa[100];
	sprintf(cosa, "31#5#255", speed);
	msg_open.CreateNullMsgOpen(); 
	msg_open.CreateMsgOpen("1", cosa ,getAddress(),"");
	emit sendFrame(msg_open.frame_open);   
}

void dimmer100::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_speed(stat_var::SPEED);
	stat_var curr_status(stat_var::ON_OFF);
	stat_var curr_fault(stat_var::FAULT);
	int val10;
	bool aggiorna = false;
	qDebug("dimmer100::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::LIGHTS:
				qDebug("Light status variation");
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				//impostaAttivo(curr_status.get_val() != 0);
				//aggiorna = true;
				if(!curr_status.get_val()) {
					// Only update on OFF
					aggiorna = true;
					impostaAttivo(0);
				} else
					impostaAttivo(1);
				break;
			case device_status::DIMMER:
				ds->read(device_status_dimmer::LEV_INDEX, curr_lev);
				qDebug("dimmer status variation, ignored");
			case device_status::DIMMER100:
				ds->read(device_status_dimmer100::LEV_INDEX, curr_lev);
				ds->read(device_status_dimmer100::SPEED_INDEX, curr_speed);
				ds->read(device_status_dimmer100::FAULT_INDEX, curr_fault);
				if(curr_fault.get_val()) {
					qDebug("DIMMER 100 FAULT !!");
					impostaAttivo(2);
				} else {
					qDebug("dimmer 100 status variation");
					qDebug("level = %d, speed = %d", curr_lev.get_val(), 
							curr_speed.get_val());
					if((curr_lev.get_val() ==0))
						impostaAttivo(0);
					if((curr_lev.get_val() <=5))
						setValue(5);
					else
						setValue(curr_lev.get_val());
					//setValue(curr_lev.get_val());
					qDebug("value = %d", getValue());
				}
				aggiorna = true ;
				break;
			case device_status::NEWTIMED:
				qDebug("new timed device status variation, ignored");
				//setValue(1);
				//aggiorna = true;
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}

void dimmer100::inizializza(bool forza)
{   
	openwebnet msg_open;
	char    pippo[50];
	qDebug("dimmer100::inizializza");

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"*1##"); 
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0])); 
	if(!forza)
		emit richStato(msg_open.frame_open);
	else
		emit sendInit(msg_open.frame_open);
}

/*****************************************************************
 **attuatAutom
 ****************************************************************/

	attuatAutom::attuatAutom( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number )
: bannOnOff( parent, name )
{       
	SetIcons( IconaSx,IconaDx ,icon, pressedIcon,period ,number );
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));                       
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_light(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));

	setChi("1");
}

#if 0
void attuatAutom::gestFrame(char* frame)
{    
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);


	if (!strcmp(msg_open.Extract_chi(),"1"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
				( !getPul() &&  (  \
						   (! strcmp(msg_open.Extract_dove(),"0")) ||   \
						   ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
						   ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
		{
			if (!strcmp(msg_open.Extract_cosa(),"1")) 		
			{
				if (!isActive())
				{
					impostaAttivo(1);
					aggiorna=1;
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"0")) 
			{
				if (isActive())
				{
					impostaAttivo(0);
					aggiorna=1;
				}
			}
		}
	}    
	if (aggiorna)
		Draw();
}
#endif

void attuatAutom::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("attuatAutom::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::LIGHTS:
				qDebug("attuatAutom status variation");
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				qDebug("status = %d", curr_status.get_val());
				if(!curr_status.get_val() && isActive()) {
					aggiorna = true;
					impostaAttivo(0);
				} else if(curr_status.get_val() && !isActive()) {
					aggiorna = true;
					impostaAttivo(1);
				}
				break;
			default:
				qDebug("attuatAutom variation, ignored");
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}


void attuatAutom::Attiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "1",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void attuatAutom::Disattiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1", "0",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}



char* attuatAutom::getChi()
{
	return("1");
}

void attuatAutom::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	if(!forza)
		emit richStato(msg_open.frame_open);
	else
		emit sendInit(msg_open.frame_open);    
}

/*****************************************************************
 **gruppo di attuatAutom
 ****************************************************************/

	grAttuatAutom::grAttuatAutom( QWidget *parent,const char *name,void *indirizzi, char* IconaDx,char* IconaSx,char *icon ,int period,int number )
: bannOnOff( parent, name )
{     
	SetIcons( IconaDx, IconaSx,NULL,icon,period ,number );
	setAddress(indirizzi);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
}



void grAttuatAutom::setAddress(void*indirizzi)
{
	elencoDisp=*((QPtrList<QString>*)indirizzi);        
	//      elencoDisp=new (QPtrList<char>*)*((QPtrList<char>*)indirizzi);        
}


void grAttuatAutom::Attiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "1",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAttuatAutom::Disattiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "0",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

/*****************************************************************
 **gruppo di dimmer
 ****************************************************************/

	grDimmer::grDimmer( QWidget *parent,const char *name,void *indirizzi, char* IconaSx,char* IconaDx,char *iconsx ,char* icondx,int period,int number )
: bannRegolaz( parent, name )
{ 
	//     setRange(1,1);
	SetIcons(  IconaSx, IconaDx ,icondx,iconsx );
	setAddress(indirizzi);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}



void grDimmer::setAddress(void*indirizzi)
{
	elencoDisp=*((QPtrList<QString>*)indirizzi);        
}


void grDimmer::Attiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "1",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grDimmer::Disattiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "0",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grDimmer::Aumenta()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "30",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grDimmer::Diminuisci()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "31",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grDimmer::inizializza(){}

/*****************************************************************
 **gruppo di dimmer100
 ****************************************************************/

	grDimmer100::grDimmer100( QWidget *parent,const char *name,void *indirizzi, char* IconaSx,char* IconaDx,char *iconsx ,char* icondx,int period,int number, QValueList<int>sstart, QValueList<int>sstop)
: grDimmer(parent, name, indirizzi, IconaSx, IconaDx, iconsx,
		icondx, period, number)
{ 
	qDebug("grDimmer100::grDimmer100()");
	qDebug("sstart[0] = %d", sstart[0]);
	soft_start = sstart;
	soft_stop = sstop;
}

void grDimmer100::Attiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++) {
		msg_open.CreateNullMsgOpen();     
		char s[100];
		sprintf(s, "*1*1#%d*%s##", soft_start[idx], 
				(elencoDisp.at(idx))->ascii());
		msg_open.CreateMsgOpen(s, strlen(s));
		emit sendFrame(msg_open.frame_open);   
	}
}

void grDimmer100::Disattiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++) {
		msg_open.CreateNullMsgOpen();     
		char s[100];
		sprintf(s, "*1*0#%d*%s##", soft_stop[idx], 
				(elencoDisp.at(idx))->ascii());
		msg_open.CreateMsgOpen(s, strlen(s));
		emit sendFrame(msg_open.frame_open);   
	}

}

void grDimmer100::Aumenta()
{
	openwebnet msg_open;
	for(uchar idx=0; idx<elencoDisp.count();idx++) {
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "30#5#255",
				(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grDimmer100::Diminuisci()
{
	openwebnet msg_open;
	for(uchar idx=0; idx<elencoDisp.count();idx++) {
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("1", "31#5#255",
				(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

//void grDimmer100::inizializza(){}

/*****************************************************************
 **scenario
 ****************************************************************/

	scenario::scenario( sottoMenu *parent,const char *name,char* indirizzo,char* IconaSx )
: bannOnSx( parent, name )
{   
	SetIcons( IconaSx,1);    
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void scenario::Attiva()
{
	openwebnet msg_open;
	char cosa[15];

	strncpy(cosa,getAddress(),sizeof(cosa));
	if (strstr(&cosa[0],"*"))
	{
		memset(index(&cosa[0],'*'),'\000',sizeof(cosa)-(index(&cosa[0],'*')-&cosa[0]));
		msg_open.CreateNullMsgOpen();          
		msg_open.CreateMsgOpen("0",&cosa[0],strstr(getAddress(),"*")+1,"");
		emit sendFrame(msg_open.frame_open);    
	}
}
void scenario::inizializza(){}
/*****************************************************************
 **carico
 ****************************************************************/

	carico::carico( sottoMenu *parent,const char *name,char* indirizzo ,char* IconaSx)
: bannOnSx( parent, name )
{

	SetIcons( IconaSx,1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void carico::gestFrame(char*)
{

}
void carico::Attiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("3", "2",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void carico::inizializza(){}
/*****************************************************************
 **attuatAutomInt
 ****************************************************************/

	attuatAutomInt::attuatAutomInt( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *StopIcon ,int period,int number )
: bannOnOff( parent, name )
{     
	memset(nomeFile1,'\000',sizeof(nomeFile1));
	memset(nomeFile2,'\000',sizeof(nomeFile2));
	strncpy(nomeFile1,icon,strstr(icon,".")-icon);
	strncpy(nomeFile2,icon,strstr(icon,".")-icon);
	strcat(nomeFile1,"o");
	strcat(nomeFile2,"c");
	strcat(nomeFile1,strstr(icon,"."));
	strcat(nomeFile2,strstr(icon,"."));
	SetIcons( IconaSx, IconaDx , icon,nomeFile1,nomeFile2);
	strncpy(nomeFile3,StopIcon,sizeof(nomeFile3));
	strncpy(nomeFile1,IconaSx,sizeof(nomeFile3));
	strncpy(nomeFile2,IconaDx,sizeof(nomeFile3));

	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(analizzaUp()));
	connect(this,SIGNAL(dxClick()),this,SLOT(analizzaDown()));

	uprunning = dorunning = 0;    
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
}

#if 0
void attuatAutomInt::gestFrame(char* frame)
{  
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);


	if (!strcmp(msg_open.Extract_chi(),"2"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
				( !getPul() &&  (  \
						   (! strcmp(msg_open.Extract_dove(),"0")) ||   \
						   ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
						   ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
		{
			if (!strcmp(msg_open.Extract_cosa(),"1")) 		
			{
				if (!isActive())
				{
					impostaAttivo(1);
					dorunning=0;
					uprunning=1;
					aggiorna=1;
					SetIcons((uchar)0,nomeFile3 );
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"0")) 
			{
				if (isActive())
				{
					impostaAttivo(0);
					uprunning=dorunning=0;
					aggiorna=1;
					SetIcons((uchar)0,nomeFile1 );
					SetIcons((uchar)1,nomeFile2 );
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"2")) 
			{
				if (!isActive())
				{
					impostaAttivo(2);
					dorunning=1;
					uprunning=0;
					aggiorna=1;
					SetIcons((uchar)1,nomeFile3 );
				}
			}
		}
	}    
	if (aggiorna)
		Draw();   
}
#else
void attuatAutomInt::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::STAT);
	bool aggiorna = false;
	qDebug("attuatAutomInt::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	int v;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::AUTOM:
				qDebug("Autom status variation");
				ds->read(device_status_autom::STAT_INDEX, curr_status);
				v = curr_status.get_val();
				qDebug("status = %d", v);
				qDebug("isActive = %d", isActive());
				switch(v) {
					case 0:
						if (isActive()) {
							impostaAttivo(0);
							uprunning=dorunning=0;
							aggiorna=1;
							SetIcons((uchar)0,nomeFile1 );
							SetIcons((uchar)1,nomeFile2 );
							sxButton->setEnabled(1);
							dxButton->setEnabled(1);
						}
						break;
					case 1:
						if (!isActive() || (isActive() == 2)) {
							impostaAttivo(1);
							dorunning=0;
							uprunning=1;
							aggiorna=1;
							SetIcons((uchar)0,nomeFile3 );
							SetIcons((uchar)1,nomeFile2 );
							dxButton->setDisabled(1);
							sxButton->setEnabled(1);
						}
						break;
					case 2:
						if (!isActive() || (isActive() == 1)) {
							impostaAttivo(2);
							dorunning=1;
							uprunning=0;
							aggiorna=1;
							SetIcons((uchar)0,nomeFile1 );
							SetIcons((uchar)1,nomeFile3 );
							sxButton->setDisabled(1);
							dxButton->setEnabled(1);
						}
						break;
					default:
						qDebug("Unknown status in autom. message");
						break;
				}
				break;
			default:
				qDebug("Unknown device status type");
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}
#endif

void attuatAutomInt::analizzaUp()
{
	if (!dorunning)
	{
		if (uprunning)
		{
			openwebnet msg_open;

			msg_open.CreateNullMsgOpen();     
			msg_open.CreateMsgOpen("2", "0",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
		}
		else
		{
			openwebnet msg_open;

			msg_open.CreateNullMsgOpen();     
			msg_open.CreateMsgOpen("2", "1",getAddress(),"");
			emit sendFrame(msg_open.frame_open);	 
		}
	}
}

void attuatAutomInt::analizzaDown()
{
	if (!uprunning)
	{
		if (dorunning)
		{
			openwebnet msg_open;

			msg_open.CreateNullMsgOpen();     
			msg_open.CreateMsgOpen("2", "0",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
		}
		else
		{
			openwebnet msg_open;

			msg_open.CreateNullMsgOpen();     
			msg_open.CreateMsgOpen("2", "2",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
		}
	}
}


void attuatAutomInt::inizializza()
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	// qDebug("mando frame attuat autom int %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    
}
/*****************************************************************
 **attuatAutomIntSic
 ****************************************************************/

	attuatAutomIntSic::attuatAutomIntSic( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *StopIcon ,int period,int number )
: bannOnOff( parent, name )
{     
	memset(nomeFile1,'\000',sizeof(nomeFile1));
	memset(nomeFile2,'\000',sizeof(nomeFile2));
	strncpy(nomeFile1,icon,strstr(icon,".")-icon);
	strncpy(nomeFile2,icon,strstr(icon,".")-icon);
	strcat(nomeFile1,"o");
	strcat(nomeFile2,"c");
	strcat(nomeFile1,strstr(icon,"."));
	strcat(nomeFile2,strstr(icon,"."));
	SetIcons( IconaSx, IconaDx , icon,nomeFile1,nomeFile2);
	strncpy(nomeFile3,StopIcon,sizeof(nomeFile3));
	strncpy(nomeFile1,IconaSx,sizeof(nomeFile3));
	strncpy(nomeFile2,IconaDx,sizeof(nomeFile3));


	setAddress(indirizzo);
	connect(this,SIGNAL(sxPressed()),this,SLOT(upPres()));
	connect(this,SIGNAL(dxPressed()),this,SLOT(doPres()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(upRil()));
	connect(this,SIGNAL(dxReleased()),this,SLOT(doRil()));

	uprunning = dorunning = 0;
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
}


#if 0
void attuatAutomIntSic::gestFrame(char* frame)
{  
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);


	if (!strcmp(msg_open.Extract_chi(),"2"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
				( !getPul() &&  (  \
						   (! strcmp(msg_open.Extract_dove(),"0")) ||   \
						   ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) ) ||     \
						   ( (! strncmp(msg_open.Extract_dove(),"#",1))  && *(getGroup()+(atoi(msg_open.Extract_dove()+1))-1) )        )   )  )
		{
			if (!strcmp(msg_open.Extract_cosa(),"1")) 		
			{
				if (!isActive())
				{
					impostaAttivo(1);
					dorunning=0;
					uprunning=1;
					aggiorna=1;
					SetIcons((uchar)0,nomeFile3 );
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"0")) 
			{
				if (isActive())
				{
					impostaAttivo(0);
					uprunning=dorunning=0;
					aggiorna=1;
					SetIcons((uchar)0,nomeFile1 );
					SetIcons((uchar)1,nomeFile2 );
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"2")) 
			{
				if (!isActive())
				{
					impostaAttivo(2);
					dorunning=1;
					uprunning=0;
					aggiorna=1;
					SetIcons((uchar)1,nomeFile3 );
				}
			}
		}
	}    
	if (aggiorna)
		Draw();   
}
#else
void attuatAutomIntSic::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::STAT);
	bool aggiorna = false;
	qDebug("attuatAutomInt::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	int v;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::AUTOM:
				qDebug("Autom status variation");
				ds->read(device_status_autom::STAT_INDEX, curr_status);
				v = curr_status.get_val();
				qDebug("status = %d", v);
				switch(v) {
					case 0:
						if (isActive()) {
							impostaAttivo(0);
							uprunning=dorunning=0;
							aggiorna=1;
							SetIcons((uchar)0,nomeFile1 );
							SetIcons((uchar)1,nomeFile2 );
							sxButton->setEnabled(1);
							dxButton->setEnabled(1);
						}
						break;
					case 1:
						if (!isActive() || (isActive() == 2)) {
							impostaAttivo(1);
							dorunning=0;
							uprunning=1;
							aggiorna=1;
							SetIcons((uchar)0,nomeFile3 );
							SetIcons((uchar)1,nomeFile2 );
							dxButton->setDisabled(1);
							sxButton->setEnabled(1);
						}
						break;
					case 2:
						if (!isActive() || (isActive() == 1)) {
							impostaAttivo(2);
							dorunning=1;
							uprunning=0;
							aggiorna=1;
							SetIcons((uchar)0,nomeFile1 );
							SetIcons((uchar)1,nomeFile3 );
							sxButton->setDisabled(1);
							dxButton->setEnabled(1);
						}
						break;
					default:
						qDebug("Unknown status in autom. message");
						break;
				}
			default:
				qDebug("Unknown device status type");
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}
#endif

void attuatAutomIntSic::upPres()
{
	if (!dorunning && !isActive())
	{
		openwebnet msg_open;

		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "1",getAddress(),"");
		emit sendFrame(msg_open.frame_open);
	} 
}

void attuatAutomIntSic::doPres()
{
	if (!uprunning && !isActive())
	{
		openwebnet msg_open;

		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "2",getAddress(),"");
		emit sendFrame(msg_open.frame_open);
	} 
}

void attuatAutomIntSic::upRil()
{
	if (!dorunning)
	{
#if 0
		openwebnet msg_open;

		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "0",getAddress(),"");
		emit sendFrame(msg_open.frame_open);
#else // 500ms delay
		QTimer::singleShot(500, this, SLOT(sendStop()) );
#endif
	} 
}

void attuatAutomIntSic::doRil()
{
	if (!uprunning)
	{
#if 0
		openwebnet msg_open;

		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "0",getAddress(),"");
		emit sendFrame(msg_open.frame_open);
#else // 500ms delay
		QTimer::singleShot(500, this, SLOT(sendStop()) );
#endif
	} 
}

void attuatAutomIntSic::sendStop()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("2", "0",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void attuatAutomIntSic::inizializza()
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	//   qDebug("mando frame attuat autom intSic %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    
}

/*****************************************************************
 ** automCancAttuatVC
 ****************************************************************/

automCancAttuatVC::automCancAttuatVC( QWidget *parent, const char *name, char* indirizzo, char* IconaSx, char* IconaDx )
: bannButIcon( parent, name )
{       
	qDebug("automCancAttuatVC::automCancAttuatVC() : "
			"%s %s", IconaSx, IconaDx);
	SetIcons(IconaSx, NULL, IconaDx);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));

	//setChi("1");
}

void automCancAttuatVC::Attiva()
{
	openwebnet msg_open;

	msg_open.CreateNullMsgOpen();  
	// FIXME: CHECK FRAME !!
	msg_open.CreateMsgOpen("6", "10",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}


/*****************************************************************
 ** automCancAttuatIll
 ****************************************************************/

automCancAttuatIll::automCancAttuatIll( QWidget *parent, const char *name, char* indirizzo, char* IconaSx, char* IconaDx, QString *t )
: bannButIcon( parent, name )
{       	  
	qDebug("automCancAttuatIll::automCancAttuatIll() : "
			"%s %s", IconaSx, IconaDx);
	SetIcons(IconaSx, NULL, IconaDx);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_autom_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
	time = t ? *t : QString("*0*0*18");
	Draw();
	//setChi("1");
}

void automCancAttuatIll::Attiva()
{
	openwebnet msg_open;
	char val[100];
	char *v[] = { val , };

	strncpy(val, time.ascii(), sizeof(val));

	msg_open.CreateNullMsgOpen();  
	//crea il messaggio open *#chi*dove*#grandezza*val_1*val_2*...val_n##
	//void openwebnet::CreateWrDimensionMsgOpen(char who[MAX_LENGTH], char where[MAX_LENGTH],
	//                                    char dimension[MAX_LENGTH],
	//                                    char** value,
	//                                    int numValue)
	msg_open.CreateWrDimensionMsgOpen("1", getAddress(), "2", v, 1);
	emit sendFrame(msg_open.frame_open);
}



/*****************************************************************
 **attuatAutomTemp
 ****************************************************************/

attuatAutomTemp::attuatAutomTemp( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number, QPtrList<QString> *lt)
: bannOnOff2scr( parent, name )
{     
	SetIcons( IconaDx, IconaSx ,icon, pressedIcon,period ,number );
	setAddress(indirizzo);
	cntTempi=0;
	static const char *t[] =  { "1'", "2'", "3'", "4'", "5'", "15'", "30''" } ;
	tempi = new QPtrList<QString>;
	tempi->clear();
	int nt = lt->count() ? lt->count() : sizeof(t) / sizeof(char *) ;
	for(int i = 0; i < nt ; i++) {
		QString *s;
		if(lt->count() && i < lt->count())
			s = lt->at(i);
		else
			s = new QString(t[i]) ;
		tempi->append(s);
	}
	//SetSeconaryText((tempi->at(cntTempi))->ascii());
	assegna_tempo_display();
	SetSeconaryText(tempo_display);
	dev = btouch_device_cache.get_light(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
	connect(this, SIGNAL(dxClick()), this, SLOT(Attiva()));
	connect(this, SIGNAL(sxClick()), this, SLOT(CiclaTempo()));
}

void attuatAutomTemp::leggi_tempo(char *&out)
{
	if(cntTempi >= ntempi())
		out = (char *)"";
	else
		out = (char *)(tempi->at(cntTempi))->ascii();
}

uchar attuatAutomTemp::ntempi()
{
	return tempi->count();
}

void attuatAutomTemp::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	int tmpval;
	bool aggiorna = false;
	qDebug("attuatAutomTemp::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	stat_var curr_status(stat_var::ON_OFF);
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::LIGHTS:
				qDebug("Light status variation");
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				aggiorna = true;
				if((isActive() && !curr_status.get_val()) ||
						(!isActive() && curr_status.get_val())) {
					// Update 
					impostaAttivo(curr_status.get_val() != 0);
				}
				break;
			case device_status::DIMMER:
				qDebug("dimmer status variation, ignored");
				break;
			case device_status::DIMMER100:
				qDebug("dimmer 100 status variation, ignored");
				break;
			case device_status::NEWTIMED:
				qDebug("new timed device status variation, ignored");
				ds->read(device_status_new_timed::HH_INDEX, curr_hh);
				ds->read(device_status_new_timed::MM_INDEX, curr_mm);
				ds->read(device_status_new_timed::SS_INDEX, curr_ss);
				if(!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val()){
					qDebug("hh == mm == ss == 0, ignoring");
					if(isActive()) {
						impostaAttivo(false);
						aggiorna = true;
					}
					break;
				}
				if(curr_hh.get_val()==255 && curr_mm.get_val()==255 && 
						curr_ss.get_val()==255) {
					qDebug("hh == mm == ss == 255, ignoring");
					break;
				}
				uchar hnow, mnow, snow;
				hnow = curr_hh.get_val();
				mnow = curr_mm.get_val();
				snow = curr_ss.get_val();
				tmpval = (hnow * 3600) + (mnow * 60) + snow;
				if((isActive() && tmpval) || (!isActive() && !tmpval)) {
					qDebug("already active, ignoring");
					break;
				}
				impostaAttivo((tmpval != 0));
				qDebug("tempo = %d %d %d", hnow, mnow, snow);
				aggiorna = true;
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}

void attuatAutomTemp::Attiva()
{
	openwebnet msg_open;
	char cosa[15];

	memset(&cosa[0],'\000',sizeof(cosa));
	sprintf(&cosa[0],"%d",(11+cntTempi));

	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("1",&cosa[0],getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void attuatAutomTemp::CiclaTempo()
{
	cntTempi= (cntTempi+1) % ntempi();
	qDebug("ntempi = %d", ntempi());
	qDebug("cntTempi = %d", cntTempi);
	assegna_tempo_display();
	SetSeconaryText(tempo_display);
	Draw();
}

void attuatAutomTemp::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	//  qDebug("mando frame attuat autom Temp %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    
}

void attuatAutomTemp::assegna_tempo_display()
{ 
	char *s ; leggi_tempo(s) ; 
	strcpy(tempo_display, s);
}

attuatAutomTemp::~attuatAutomTemp()
{
	int i;
	for(i=0; i<tempi->count(); i++)
		delete tempi->at(i);
	delete tempi;
}

/*****************************************************************
 **attuatAutomTempNuovoN
 ****************************************************************/

	attuatAutomTempNuovoN::attuatAutomTempNuovoN( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *pressedIcon ,int period,int number , QPtrList<QString> *lt)
: attuatAutomTemp( parent, name, indirizzo, IconaSx, IconaDx, icon,
		pressedIcon, period, number, lt)
{     
	assegna_tempo_display();
	stato_noto = false ;
	SetSeconaryText(tempo_display);
	disconnect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(attuatAutomTemp::status_changed(QPtrList<device_status>)));
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
}

void attuatAutomTempNuovoN::status_changed(QPtrList<device_status> sl)
{
	bool aggiorna = false;
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	int tmpval;
	qDebug("attuatAutomTempNuovoN::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	stat_var curr_status(stat_var::ON_OFF);
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::LIGHTS:
				qDebug("Light status variation");
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				aggiorna = true;
				if((isActive() && !curr_status.get_val()) ||
						(!isActive() && curr_status.get_val())) {
					// Update 
					impostaAttivo(curr_status.get_val() != 0);
				}
				break;
			case device_status::DIMMER:
				qDebug("dimmer status variation, ignored");
				break;
			case device_status::DIMMER100:
				qDebug("dimmer 100 status variation, ignored");
				break;
			case device_status::NEWTIMED:
				qDebug("new timed device status variation");
				ds->read(device_status_new_timed::HH_INDEX, curr_hh);
				ds->read(device_status_new_timed::MM_INDEX, curr_mm);
				ds->read(device_status_new_timed::SS_INDEX, curr_ss);
				if(!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val()){
					qDebug("hh == mm == ss == 0, ignoring");
					if(isActive()) {
						impostaAttivo(false);
						aggiorna = true;
					}
					break;
				}
				if(curr_hh.get_val()==255 && curr_mm.get_val()==255 && 
						curr_ss.get_val()==255) {
					qDebug("hh == mm == ss == 255, ignoring");
					break;
				}
				uchar hnow, mnow, snow;
				hnow = curr_hh.get_val();
				mnow = curr_mm.get_val();
				snow = curr_ss.get_val();
				tmpval = (hnow * 3600) + (mnow * 60) + snow;
				if((isActive() && tmpval) || (!isActive() && !tmpval)) {
					qDebug("already active, ignoring");
					break;
				}
				impostaAttivo((tmpval != 0));
				qDebug("tempo = %d %d %d", hnow, mnow, snow);
				aggiorna = true;
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	qDebug("aggiorna = %d", aggiorna);
	if(aggiorna)
		Draw();
	delete dsi;
}

void attuatAutomTempNuovoN::Attiva()
{
	openwebnet msg_open;
	char frame[100];
	char *t ; leggi_tempo(t);
	sprintf(frame, "*#1*%s*#2*%s##", getAddress(), t);
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen(frame, strlen(frame));
	emit sendFrame(msg_open.frame_open);
}

// *#1*dove*2## 
void attuatAutomTempNuovoN::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"*2##");
	//  qDebug("mando frame attuat autom Temp %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    
}

void attuatAutomTempNuovoN::assegna_tempo_display()
{
	char tmp[50];
	int hh , mm, ss;
	char *ptr;
	leggi_tempo(ptr);
	strcpy(tmp, ptr);
	// Prende solo hh e min
	ptr = strtok(tmp, "*");
	hh = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	mm = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	ss = strtol(ptr, NULL, 10);
	qDebug("tempo = %d %d %d", hh, mm, ss);
	if(!hh && !mm) {
		// Time in secs
		sprintf(tempo_display, "%d\"", ss);
	} else if (!hh) {
		// Time in mins'
		sprintf(tempo_display, "%d'", mm);
	} else if(hh < 10) {
		// Time in hh:mm
		sprintf(tempo_display, "%d:%02d", hh, mm);
	} else {
		// Time in hh h
		sprintf(tempo_display, "%dh", hh);
	}
}


/*****************************************************************
 **attuatAutomTempNuovoF
 ****************************************************************/

#define NTIMEICONS 9

	attuatAutomTempNuovoF::attuatAutomTempNuovoF( QWidget *parent,const char *name,char* indirizzo,char* IconaCentroSx,char* IconaCentroDx,char *IconaDx, const char *t)
: bannOn2scr( parent, name)
{     
	attuatAutomTempNuovoF::SetIcons( IconaCentroSx, IconaCentroDx, IconaDx );
	setAddress(indirizzo);
	SetSeconaryText("????");
	strncpy(tempo, t ? t : "0*0*0", sizeof(tempo));
	char *ptr ; 
	char tmp1[50];
	strcpy(tmp1, tempo);
	ptr = strtok(tmp1, "*");
	h = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	m = strtol(ptr, NULL, 10);
	ptr = strtok(NULL, "*");
	s = strtol(ptr, NULL, 10);
	val = 0;
	qDebug("tempo = %d %d %d", h, m, s);
	char tmp[50];
	if(!h && !m) {
		// Time in secs
		sprintf(tmp, "%d\"", s);
	} else if (!h) {
		// Time in mins'
		sprintf(tmp, "%d'", m);
	} else if(h < 10) {
		// Time in hh:mm
		sprintf(tmp, "%d:%02d", h, m);
	} else {
		// Time in hh h
		sprintf(tmp, "%dh", h);
	}
	myTimer = new QTimer(this,"periodic_refresh");
	stato_noto = false ;
	temp_nota = false ;
	connect(myTimer,SIGNAL(timeout()),this,SLOT(update()));
	update_ok = false;
	tentativi_update = 0;
	SetSeconaryText(tmp);
	connect(this,SIGNAL(dxClick()),this,SLOT(Attiva())); 
	// Crea o preleva il dispositivo dalla cache
	device *dev = btouch_device_cache.get_newtimed(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
	dev = btouch_device_cache.get_dimmer(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)),
			this, SLOT(status_changed(QPtrList<device_status>)));
}

void attuatAutomTempNuovoF::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_hh(stat_var::HH);
	stat_var curr_mm(stat_var::MM);
	stat_var curr_ss(stat_var::SS);
	stat_var curr_status(stat_var::ON_OFF);
	int val10;
	bool aggiorna = false;
	qDebug("attuatAutomTempNuovoF::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::LIGHTS:
				qDebug("Light status variation");
				aggiorna = true;
				ds->read(device_status_light::ON_OFF_INDEX, curr_status);
				qDebug("status = %d", curr_status.get_val());
				if((isActive() && !curr_status.get_val()) ||
						(!isActive() && curr_status.get_val())) {
					impostaAttivo(curr_status.get_val() != 0);
					if(!isActive())
						myTimer->stop();
					val = 0;
				}
				break;
			case device_status::DIMMER:
				qDebug("dimmer status variation, ignored");
				break;
			case device_status::DIMMER100:
				qDebug("dimmer 100 status variation, ignored");
				break;
			case device_status::NEWTIMED:
				qDebug("new timed device status variation");
				ds->read(device_status_new_timed::HH_INDEX, curr_hh);
				ds->read(device_status_new_timed::MM_INDEX, curr_mm);
				ds->read(device_status_new_timed::SS_INDEX, curr_ss);
				if(!curr_hh.get_val() && !curr_mm.get_val() && !curr_ss.get_val()){
					qDebug("hh == mm == ss == 0, ignoring");
					myTimer->stop();
					val = 0;
					break;
				}
				if(curr_hh.get_val()==255 && curr_mm.get_val()==255 && 
						curr_ss.get_val()==255) {
					qDebug("hh == mm == ss == 255, ignoring");
					break;
				}
				uchar hnow, mnow, snow;
				hnow = curr_hh.get_val();
				mnow = curr_mm.get_val();
				snow = curr_ss.get_val();
				int tmpval = (hnow * 3600) + (mnow * 60) + snow;
				if(tmpval == val) return;
				val = tmpval ;
				impostaAttivo((val != 0));
				update_ok = true;
				if(!val) 
					myTimer->stop();
				else if(!myTimer->isActive())
					myTimer->start((1000 * val) / NTIMEICONS);
				qDebug("tempo = %d %d %d", hnow, mnow, snow);
				aggiorna = true;
				break;
		}
		++(*dsi);
	}
	if(aggiorna) {
		qDebug("invoco Draw con value = %d", value);
		Draw();
	}
	delete dsi;
}

void attuatAutomTempNuovoF::update()
{
	if(!update_ok) {
		if(tentativi_update++ > 2) {
			qDebug("Esauriti i tentativi di aggiornamento\n");
			myTimer->stop();
			return;
		}
		update_ok = false;
	}
	openwebnet msg_open;
	char frame[100];
	sprintf(frame, "*#1*%s*2##", getAddress());
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen(frame, strlen(frame));
	emit sendInit(msg_open.frame_open);
}

void attuatAutomTempNuovoF::Attiva()
{
	openwebnet msg_open;
	char frame[100];
	char *t ; leggi_tempo(t);
	sprintf(frame, "*#1*%s*#2*%s##", getAddress(), t);
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen(frame, strlen(frame));
	emit sendFrame(msg_open.frame_open);
	// Chiede subito la temporizzazione
	update();
	// Valore iniziale = il valore impostato
	int v = h * 3600 + m * 60 + s;
	// e programma un aggiornamento
	tentativi_update = 1;
	myTimer->start((1000 * v) / NTIMEICONS );
	Draw();
}

// *#1*dove*2## 
void attuatAutomTempNuovoF::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"*2##");
	//  qDebug("mando frame attuat autom Temp %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    
}

// Chiede lo stato dell'attuatore con una frame vecchia
void attuatAutomTempNuovoF::chiedi_stato()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#1*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit richStato(msg_open.frame_open);    
}

void attuatAutomTempNuovoF::leggi_tempo(char *&out)
{
	out = tempo;
}

void attuatAutomTempNuovoF::SetIcons(char *i1, char *i2, char *i3)
{
	qDebug("attuatAutomTempNuovoF::SetIcons");
	char tmp[MAX_PATH], tmp1[MAX_PATH] ;
	char *ptr;
	char pressIconName[MAX_PATH];
	if(!Icon[0])
		Icon[0] = new QPixmap();
	strcpy(tmp1, i2);
	ptr = strtok(tmp1, ".");
	sprintf(tmp, "%soff.png", ptr);
	Icon[0]->load(tmp);
	qDebug("Icon[0] <- %s", tmp);
	if(!Icon[1])
		Icon[1] = new QPixmap();
	strcpy(tmp1, i2);
	ptr = strtok(tmp1, ".");
	sprintf(tmp, "%son.png", ptr);
	Icon[1]->load(tmp);
	qDebug("Icon[1] <- %s", tmp);
	if(!Icon[2])
		Icon[2] = new QPixmap();
	Icon[2]->load(i3);
	qDebug("Icon[2] <- %s", i3);
	getPressName(i3, pressIconName, sizeof(pressIconName));
	if (QFile::exists(pressIconName)) {
		if(!pressIcon[2])
			pressIcon[2] = new QPixmap();
		pressIcon[2]->load(pressIconName);
		qDebug("pressIcon[2] <- %s", pressIconName);
	}
	for(int i = 0; i < NTIMEICONS ; i++) {
		if(!Icon[3 + i])
			Icon[3 + i] = new QPixmap();
		strcpy(tmp1, i1);
		ptr = strtok(tmp1, ".");
		sprintf(tmp, "%s%d.png", ptr, i);
		Icon[3 + i]->load(tmp);	
		qDebug("Icon[%d] <- %s", 3+i, tmp);
	}
}

void attuatAutomTempNuovoF::Draw()
{
	qDebug("attuatAutomTempNuovoF::Draw(), attivo = %d, value = %d", 
			attivo, val);
	if (attivo == 1) {
		int index = ((10 * val * (NTIMEICONS-1))/((h * 3600) + (m * 60) + s)) ;
		index = (index % 10) >= 5 ? index/10 + 1 : index/10;
		if(index >= NTIMEICONS)
			index = NTIMEICONS - 1;
		qDebug("index = %d", index);
		if (Icon[3 + index] && BannerIcon2) {
			BannerIcon->setPixmap(*Icon[3 + index]);
			qDebug("* Icon[%d]", 3 + index);
		}
		if (Icon[1] && BannerIcon) {
			BannerIcon2->setPixmap(*Icon[1]);
			qDebug("** Icon[%d]", 1);
		}
	} else {
		if (Icon[0] && BannerIcon) {
			BannerIcon->setPixmap(*Icon[3]);
			qDebug("*** Icon[3]");
		}
		if (Icon[3] && BannerIcon2) {
			BannerIcon2->setPixmap(*Icon[0]);
			qDebug("**** Icon[0]");
		}
	}
	if ( (dxButton) && (Icon[2]) ) {
		dxButton->setPixmap(*Icon[2]);
		if (pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if (BannerText) {
		BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
		BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
		BannerText->setText(testo);
	}
	if (SecondaryText) {	
		SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
		SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
		SecondaryText->setText(testoSecondario);
	} 
}


/*****************************************************************
 **gruppo di attuatInt
 ****************************************************************/

	grAttuatInt::grAttuatInt( QWidget *parent,const char *name,void *indirizzi,char* IconaSx,char* IconaDx, char *icon ,int period,int number )
: bann3But( parent, name )
{     
	SetIcons(  IconaDx,IconaSx ,NULL,icon,period ,number );
	setAddress(indirizzi);
	connect(this,SIGNAL(dxClick()),this,SLOT(Alza()));
	connect(this,SIGNAL(sxClick()),this,SLOT(Abbassa()));
	connect(this,SIGNAL(centerClick()),this,SLOT(Ferma()));
}



void grAttuatInt::setAddress(void*indirizzi)
{
	elencoDisp=*((QPtrList<QString>*)indirizzi);
}


void grAttuatInt::Alza()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "1",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAttuatInt::Abbassa()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "2",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAttuatInt::Ferma()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("2", "0",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAttuatInt::inizializza()
{ 
}
/*****************************************************************
 **attuatPuls
 ****************************************************************/

	attuatPuls::attuatPuls( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,/*char* IconaDx,*/char *icon ,char tipo ,int period,int number )
: bannPuls( parent, name )
{               
	SetIcons(  IconaSx,NULL,icon,NULL,period ,number );
	setAddress(indirizzo);
	connect(this,SIGNAL(sxPressed()),this,SLOT(Attiva()));
	connect(this,SIGNAL(sxReleased()),this,SLOT(Disattiva()));
	type=tipo;
	impostaAttivo(1);
}



void attuatPuls::Attiva()
{
	openwebnet msg_open;
	switch (type) {
		case  AUTOMAZ: 
			msg_open.CreateNullMsgOpen();
			msg_open.CreateMsgOpen("1", "1",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
			break;
		case  VCT_SERR:
			msg_open.CreateNullMsgOpen();
			msg_open.CreateMsgOpen("6", "10",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
			break;	 
		case  VCT_LS:
			msg_open.CreateNullMsgOpen();
			msg_open.CreateMsgOpen("6", "12",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
			break;
	}
}

void attuatPuls::Disattiva()
{
	openwebnet msg_open;
	switch (type) {
		case  AUTOMAZ:
			msg_open.CreateNullMsgOpen();     
			msg_open.CreateMsgOpen("1", "0",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
			break;	 
		case  VCT_SERR: 
			/*  msg_open.CreateNullMsgOpen();     
			    msg_open.CreateMsgOpen("6", "1",getAddress(),"");
			    emit sendFrame(msg_open.frame_open);*/
			break;	 
		case  VCT_LS: 
			msg_open.CreateNullMsgOpen();     
			msg_open.CreateMsgOpen("6", "11",getAddress(),"");
			emit sendFrame(msg_open.frame_open);
			break;	 
	}  
}

void attuatPuls::inizializza()
{ 
}
/*****************************************************************
 **amplificatore
 ****************************************************************/

	amplificatore::amplificatore( QWidget *parent,const char *name,char* indirizzo,char* IconaSx,char* IconaDx,char *icon ,char *inactiveIcon )
: bannRegolaz( parent, name )
{ 
	qDebug("amplificatore::amplificatore()");
	setRange(1,9);
	SetIcons( IconaSx, IconaDx ,icon, inactiveIcon,(char)1 );
	qDebug("%s - %s - %s - %s", IconaSx, IconaDx, icon, inactiveIcon);
	setAddress(indirizzo);
	connect(this,SIGNAL(sxClick()),this,SLOT(Accendi()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Spegni()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
	setValue(1);
	impostaAttivo(0);
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_sound_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
}

int trasformaVol(int vol)
{
	if (vol<0)
		return(-1);

	if (vol<=3) 
		return(1);
	if (vol<=7) 
		return(2);
	if (vol<=11) 
		return(3);
	if (vol<=14) 
		return(4);
	if (vol<=17) 
		return(5);
	if (vol<=20) 
		return(6);
	if (vol<=23) 
		return(7);
	if (vol<=27) 
		return(8);
	if (vol<=31) 
		return(9);
	return(-1);
}

#if 0
void amplificatore::gestFrame(char* frame)
{
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"16"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),getAddress()) ) ||     \
				( !getPul() &&  (  \
						   (! strcmp(msg_open.Extract_dove(),"0")) ||   \
						   ( ( strlen(msg_open.Extract_dove())==1) && (! strncmp(msg_open.Extract_dove(),getAddress(),1)) )          )   )  )
		{
			if (!msg_open.IsMeasureFrame())
			{
				if (!strcmp(msg_open.Extract_cosa(),"13")) 		
				{
					if (isActive())
					{
						impostaAttivo(0);
						aggiorna=1;
					}
				}
				else if ((!strcmp(msg_open.Extract_cosa(),"0")) || (!strcmp(msg_open.Extract_cosa(),"3")))
				{
					if (!isActive())
					{
						impostaAttivo(1);
						aggiorna=1;
					}
				}
			}
			else
			{
				if ( !strcmp(msg_open.Extract_grandezza(),"1")) 
				{
					int vol;

					vol=atoi(msg_open.Extract_valori(0))&0x1F;
					vol=trasformaVol(vol);
					setValue(vol);
					qDebug("imposto livello : %d",vol);		    
					aggiorna=1;
				}
			}
		}
	}    
	if (aggiorna)
		Draw();
}
#else
void amplificatore::status_changed(QPtrList<device_status>sl)
{
	stat_var curr_lev(stat_var::LEV);
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("amplificatore::status_changed");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::AMPLIFIER :
				qDebug("Ampli status variation");
				ds->read(device_status_amplifier::ON_OFF_INDEX, curr_status);
				ds->read(device_status_amplifier::AUDIO_LEVEL_INDEX, curr_lev);
				qDebug("status = %d, lev = %d", curr_status.get_val(), 
						curr_lev.get_val());
				if((isActive() && !curr_status.get_val()) ||
						(!isActive() && curr_status.get_val())) {
					impostaAttivo(curr_status.get_val() != 0);
					aggiorna = true;
				}
				if(getValue() != curr_lev.get_val()) {
					setValue(trasformaVol(curr_lev.get_val()));
					aggiorna = true ;
				}
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}
#endif


void amplificatore:: Accendi()
{
	openwebnet msg_open;

	qDebug("amplificatore::Accendi()");
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "3",getAddress(),"");
	emit sendFrame(msg_open.frame_open);     
}
void amplificatore:: Spegni()
{
	openwebnet msg_open;

	qDebug("amplificatore::Spegni()");
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "13",getAddress(),"");
	emit sendFrame(msg_open.frame_open);     
}
void amplificatore:: Aumenta()
{
	openwebnet msg_open;

	qDebug("amplificatore::Aumenta()");
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "1001",getAddress(),"");
	emit sendFrame(msg_open.frame_open);     
}
void amplificatore:: Diminuisci()	
{
	openwebnet msg_open;

	qDebug("amplificatore::Diminuisci()");
	msg_open.CreateNullMsgOpen();     
	msg_open.CreateMsgOpen("16", "1101",getAddress(),"");
	emit sendFrame(msg_open.frame_open);     
}
void amplificatore::inizializza()
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*1##");
	//   qDebug("mando frame ampli %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*5##");
	//   qDebug("mando frame ampli %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    

}
/*****************************************************************
 **gruppo di amplificatori
 ****************************************************************/

	grAmplificatori::grAmplificatori( QWidget *parent,const char *name,void *indirizzi,char* IconaSx,char* IconaDx, char *iconsx ,char* icondx,int period,int number )
: bannRegolaz( parent, name )
{ 
	//     setRange(1,1);
	SetIcons(IconaSx,IconaDx ,icondx,iconsx );
	setAddress(indirizzi);
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}



void grAmplificatori::setAddress(void*indirizzi)
{
	elencoDisp=*((QPtrList<QString>*)indirizzi);    
#if 0
	QPtrListIterator<QString> *lii =
		new QPtrListIterator<QString>(elencoDisp);
	QString *s;
	qDebug("Indirizzi gruppi amplificatori:");
	while( ( s = lii->current()) ) {
		qDebug("INDIRIZZO = %s", s->ascii());
		++(*lii);
	}
	delete lii;
#endif
}


void grAmplificatori::Attiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("16", "3",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAmplificatori::Disattiva()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("16", "13",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAmplificatori::Aumenta()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("16", "1001",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}

void grAmplificatori::Diminuisci()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();     
		msg_open.CreateMsgOpen("16", "1101",(char*)elencoDisp.at(idx)->ascii(),"");
		emit sendFrame(msg_open.frame_open);
	}
}
void grAmplificatori::inizializza()
{ 
}

/*****************************************************************
 **sorgente_aux
 ****************************************************************/
sorgente_aux::sorgente_aux( QWidget *parent,const char *name,char* indirizzo, bool vecchio, char *ambdescr)
: bannCiclaz( parent, name, vecchio ? 4 : 3)
{
	SetIcons( ICON_CICLA,NULL,ICON_FFWD,ICON_REW);

	vecchia = vecchio;
	setAddress(indirizzo);
	//     connect(this,SIGNAL(click()),this,SLOT(Attiva()));
	if(vecchia) {
		connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
		nascondi(BUT2);
	} else {
		myAux = new aux(NULL, name, ambdescr);
		myAux->setBGColor(parentWidget(TRUE)->backgroundColor() );
		myAux->setFGColor(parentWidget(TRUE)->foregroundColor() );
		// Get freezed events
		connect(parent, SIGNAL(frez(bool)), myAux, SLOT(freezed(bool)));
	}
}

void sorgente_aux::gestFrame(char*)
{
}

void sorgente_aux::ciclaSorg()
{
	openwebnet msg_open;
	msg_open.CreateMsgOpen("16","23","100","");
	emit sendFrame(msg_open.frame_open);
}

void sorgente_aux::decBrano()
{
	openwebnet msg_open;
	msg_open.CreateMsgOpen("16","6101",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void sorgente_aux::aumBrano()
{
	openwebnet msg_open;
	char amb[2];
	sprintf(amb, getAddress());
	if(!vecchia)
		if(amb[1] == '0')
			amb[1] = '1';

	msg_open.CreateMsgOpen("16","6001",amb,"");
	emit sendFrame(msg_open.frame_open);
}

void sorgente_aux::inizializza()
{
}

void sorgente_aux::hide()
{
	qDebug("sorgente::hide()");
	banner::hide();
	if(vecchia)
		return;
	myAux->hide();
}


/*****************************************************************
 **SorgenteMultimedia
 ****************************************************************/
BannerSorgenteMultimedia::BannerSorgenteMultimedia(QWidget *parent, const char *name, char *indirizzo, int where) :
	bannCiclaz(parent, name, 4),
	source_menu(NULL, name, indirizzo, where)
{
	SetIcons(ICON_CICLA, ICON_IMPOSTA, ICON_FFWD, ICON_REW);

	setAddress(indirizzo);

	source_menu.setBGColor(parentWidget(TRUE)->backgroundColor());
	source_menu.setFGColor(parentWidget(TRUE)->foregroundColor());

	connect(this, SIGNAL(dxClick()), &source_menu, SLOT(showAux()));

	// Get freezed events
	// FIXME: what's this?
	connect(parent, SIGNAL(frez(bool)), &source_menu, SLOT(freezed(bool)));

	QWidget *sotto_menu = this->parentWidget(FALSE)->parentWidget(FALSE);
	connect(&source_menu, SIGNAL(Closed()), sotto_menu, SLOT(show()));
	connect(&source_menu, SIGNAL(Closed()), &source_menu, SLOT(hide()));
	connect(&source_menu, SIGNAL(sendFrame(char *)), this, SIGNAL(sendFrame(char *)));
}

void BannerSorgenteMultimedia::ciclaSorg()
{
}

void BannerSorgenteMultimedia::decBrano() {}
void BannerSorgenteMultimedia::aumBrano() {}
void BannerSorgenteMultimedia::menu() {}
void BannerSorgenteMultimedia::gestFrame(char *) {}

void BannerSorgenteMultimedia::hide() 
{
	banner::hide();
	source_menu.hide();
}


// void BannerSorgenteMultimedia::inizializza()
// {
// 	source_menu.initAudio();
// }

/*****************************************************************
 **sorgente_Radio
 ****************************************************************/
banradio::banradio( QWidget *parent,const char *name,char* indirizzo, int nbut, char *ambdescr)
: bannCiclaz( parent, name, nbut )
{
	SetIcons( ICON_CICLA,ICON_IMPOSTA,ICON_FFWD,ICON_REW);
	setAddress(indirizzo);
	myRadio = new radio(NULL,"radio", ambdescr);
	myRadio->setRDS("");
	myRadio->setFreq(0.00);
	//myRadio->setAmbDescr(ambdescr);

	//     myRadio-> setBGColor(parentWidget(TRUE)->backgroundColor() );
	//     myRadio-> setFGColor(parentWidget(TRUE)->foregroundColor() );	
	QWidget *grandad = this->parentWidget(FALSE)->parentWidget(FALSE);

	myRadio->setStaz((uchar)1);

	if(nbut == 4) 
	{
		// Old difson
		old_diffson = true;
		connect(this  ,SIGNAL(sxClick()),this,SLOT(ciclaSorg()));
		connect(this  ,SIGNAL(csxClick()),this,SLOT(decBrano()));
		connect(this  ,SIGNAL(cdxClick()),this,SLOT(aumBrano()));
	} 
	else
		old_diffson = false;

	connect(this  ,SIGNAL(dxClick()),myRadio,SLOT(showRadio()));
	connect(this , SIGNAL(dxClick()),this,SLOT(startRDS()));
	if(!old_diffson)
		connect(this, SIGNAL(dxClick()), this, SLOT(richFreq()));

	//connect(this  ,SIGNAL(dxClick()), grandad, SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()), grandad, SLOT(show()));
	connect(myRadio,SIGNAL(Closed()),myRadio,SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()),this,SLOT(stopRDS()));     

	connect(myRadio,SIGNAL(decFreqAuto()),this,SLOT(decFreqAuto()));
	connect(myRadio,SIGNAL(aumFreqAuto()),this,SLOT(aumFreqAuto()));
	connect(myRadio,SIGNAL(decFreqMan()),this,SLOT(decFreqMan()));
	connect(myRadio,SIGNAL(aumFreqMan()),this,SLOT(aumFreqMan()));
	connect(myRadio,SIGNAL(changeStaz()),this,SLOT(changeStaz()));
	connect(myRadio,SIGNAL(memoFreq(uchar)),this,SLOT(memoStaz(uchar)));
	connect(myRadio,SIGNAL(richFreq()),this,SLOT(richFreq()));

	//     myRadio->hide();
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_radio_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));

	// Get freezed events
	connect(parent, SIGNAL(frez(bool)), myRadio, SLOT(freezed(bool)));

}

void banradio::grandadChanged(QWidget *newGrandad)
{
	qDebug("banradio::grandadChanged (%p)", newGrandad); 
	QWidget *grandad = this->parentWidget(FALSE)->parentWidget(FALSE);
	//disconnect(this  ,SIGNAL(dxClick()), grandad, SLOT(hide()));
	disconnect(myRadio,SIGNAL(Closed()), grandad, SLOT(showFullScreen()));
	grandad = newGrandad;
	//connect(this  ,SIGNAL(dxClick()), grandad, SLOT(hide()));
	connect(myRadio,SIGNAL(Closed()), grandad, SLOT(showFullScreen()));
}


#if 0
void banradio::gestFrame(char*frame)
{
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);

	if (!strcmp(msg_open.Extract_chi(),"16"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),getAddress()) )  )
		{
			if (!msg_open.IsMeasureFrame())
			{
			}
			else
			{
				if ( !strcmp(msg_open.Extract_grandezza(),"6")) 
				{
					float freq;

					freq=atoi(msg_open.Extract_valori(1));
					freq=freq/1000;
					myRadio->setFreq(freq);
					myRadio->setRDS("");
					aggiorna=1;
				}
				if ( !strcmp(msg_open.Extract_grandezza(),"7")) 
				{
					int staz;

					staz=atoi(msg_open.Extract_valori(1));
					myRadio->setStaz((uchar)staz);
					aggiorna=1;
				}
				if ( !strcmp(msg_open.Extract_grandezza(),"8")) 
				{
					char rds[9];
					for(int idx=0;idx<8;idx++)
					{
						rds[idx]=atoi(msg_open.Extract_valori(idx));
					}
					rds[8]='\000';		    
					myRadio->setRDS(&rds[0]);
					aggiorna=1;
				}
			}
		}
	}    
	if (aggiorna)
		myRadio->draw();
}
#else
void banradio::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_freq(stat_var::FREQ);
	stat_var curr_staz(stat_var::STAZ);
	stat_var curr_rds0(stat_var::RDS0);
	stat_var curr_rds1(stat_var::RDS1);
	int val10;
	bool aggiorna = false;
	qDebug("bannradio::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	int tmp;
	float freq;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::RADIO:
				qDebug("Radio status variation");
				ds->read(device_status_radio::FREQ_INDEX, curr_freq);
				ds->read(device_status_radio::STAZ_INDEX, curr_staz);
				ds->read(device_status_radio::RDS0_INDEX, curr_rds0);
				ds->read(device_status_radio::RDS1_INDEX, curr_rds1);
				freq = (float)curr_freq.get_val()/1000.0F;
				myRadio->setFreq(freq);
				myRadio->setStaz((uchar)curr_staz.get_val());
				char rds[9];
				tmp = curr_rds0.get_val();
				qDebug("rds0 = 0x%08x", tmp);
				memcpy((void *)rds, (void *)&tmp, 4);
				tmp = curr_rds1.get_val();
				qDebug("rds1 = 0x%08x", tmp);
				memcpy((void *)&rds[4], (void *)&tmp, 4);
				qDebug("*** setting rds to %s", rds);
				myRadio->setRDS(rds);
				aggiorna=1;
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		myRadio->draw();
	delete dsi;
}
#endif

void banradio::pre_show()
{
	openwebnet msg_open;
	char    pippo[50];

	if(old_diffson) {
		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*#16*");
		strcat(pippo,getAddress());
		strcat(pippo,"*6##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

		emit sendInit(msg_open.frame_open);
	}

	QWidget::show();
}

void banradio::show()
{

	pre_show();

	if(!old_diffson)
	{
		if (parentWidget()->parentWidget()->parentWidget(TRUE))
			nascondi(BUT2);
		else
			mostra(BUT2);
	}
	mostra(BUT2);
}

void banradio::hide()
{
	qDebug("banradio::hide()");
	if (myRadio->isShown())
		stopRDS();
	myRadio->hide();
	QWidget::hide();
}

void banradio::SetText( const char *text )
{
	/*    memset(testo,'\000',sizeof(testo));
	      strncpy(testo,text,MAX_TEXT-1);*/
	banner::SetText(text);
	myRadio->setName((char*)text);
}

void banradio::ciclaSorg()
{
	openwebnet msg_open;
	qDebug("banradio::ciclaSorg()");
	msg_open.CreateMsgOpen("16","23","100","");
	emit sendFrame(msg_open.frame_open);
}

void banradio::decBrano()
{
	openwebnet msg_open;

	msg_open.CreateMsgOpen("16","6101",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void banradio::aumBrano()
{
	openwebnet msg_open;

	msg_open.CreateMsgOpen("16","6001",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}
void banradio::aumFreqAuto()
{
	openwebnet msg_open;

	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5000",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}
void banradio::decFreqAuto()
{
	openwebnet msg_open;

	myRadio->setFreq(0.00);
	myRadio->setRDS("- - - - ");
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5100",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
}

void banradio::aumFreqMan()
{
	/*   static bool flag;

	     if (flag)
	     {*/
	openwebnet msg_open;
	float f;

	f=myRadio->getFreq();
	if (f<108.00)
		f+=0.05;
	else
		f=87.50;
	myRadio->setFreq(f);
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5001",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
	/*	flag=FALSE;
		}
		else
		flag=TRUE;*/
}

void banradio::decFreqMan()
{
	/*  static bool flag;

	    if (flag)
	    {*/
	openwebnet msg_open;
	float f;

	f=myRadio->getFreq();
	if (f>87.50)
		f-=0.05;
	else
		f=108.00;
	myRadio->setFreq(f);
	myRadio->draw();
	msg_open.CreateMsgOpen("16","5101",getAddress(),"");
	emit sendFrame(msg_open.frame_open);
	/* 	flag=FALSE;
		}
		else
		flag=TRUE;*/
}
void banradio::changeStaz()
{
	openwebnet msg_open;
	char amb[2];

	sprintf(amb, getAddress());
	if(!old_diffson)
		if(amb[1] == '0')
			amb[1] = '1';

	msg_open.CreateMsgOpen("16","6001",amb,"");
	emit sendFrame(msg_open.frame_open);
}

void banradio::memoStaz(uchar st)
{
	openwebnet msg_open;
	char    pippo[50],pippa[10];
	unsigned int ic;
	// double f;

	memset(pippo,'\000',sizeof(pippo));
	memset(pippa,'\000',sizeof(pippa));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*#10*");
	memset(pippa,'\000',sizeof(pippa));
	//     f=myRadio->getFreq()*1000;
	//     sprintf(pippa,"%.0f",f);
	//     qDebug("pippa2  :%s",&pippa[0]);
	//     strcat(pippo,pippa);
	//     strcat(pippo,"*");
	ic=(unsigned int)st;
	sprintf(pippa,"%01hu",st);
	//   qDebug("pippa1  :%01hu",&pippa[0]);
	strcat(pippo,pippa);
	strcat(pippo,"##");
	qDebug(msg_open.frame_open);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	emit sendFrame(msg_open.frame_open);
}
void banradio::startRDS()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*101*");
	strcat(pippo,getAddress());
	//     strcat(pippo,"*8##");
	strcat(pippo,"##");

	qDebug(msg_open.frame_open);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	emit sendFrame(msg_open.frame_open);
}

void banradio::stopRDS()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*16*102*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");

	qDebug(msg_open.frame_open);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	emit sendFrame(msg_open.frame_open);
}

void banradio::richFreq()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*6##");

	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));

	emit sendFrame(msg_open.frame_open);
}

void banradio::setBGColor(int r, int g , int b)
{
	setBGColor(QColor(r,g,b));
}
void banradio::setFGColor(int r, int g, int b)
{
	setFGColor(QColor(r,g,b));
}
void banradio::setBGColor(QColor c)
{
	if (c.isValid())
	{
		myRadio->setBGColor(c);
		banner::setBGColor(c);
	}
}
void banradio::setFGColor(QColor c)
{
	if (c.isValid())
	{
		myRadio->setFGColor(c);
		banner::setFGColor(c);
	}
}



void banradio::inizializza()
{ 
}



/*****************************************************************
 **termoPage
 ****************************************************************/
termoPage::termoPage(QWidget *parent, int tipo_sottomenu, const char *name , char* indirizzo,
		QPtrList<QString> &icon_names,
		//char *IconaMeno, char *IconaPiu, char *IconaMan, char *IconaAuto, char *IconaAntigelo, char *IconaOff,
		QColor SecondForeground, int type, const QString &ind_centrale)
	: bannTermo( parent, name, SecondForeground )
{
	/// FIXTHERMO
	/// this is to support new addressing for termo central
	this->type = type;
	this->ind_centrale = ind_centrale;
	QString new_address;
	if (type == 0)
		new_address = QString("%1").arg( indirizzo );
	else
		new_address = QString("%1#%2").arg( indirizzo ).arg( ind_centrale );
	setAddress( const_cast<char *>(new_address.ascii()) );

	qDebug(QString(">>> thermoPage -> INDIRIZZO VECCHIO: %1 <<<").arg(indirizzo));
	qDebug(QString(">>> Creating TermoPage Item with type=%1 and ind_centrale=%2 <<<").arg(type).arg(ind_centrale));
	qDebug(QString(">>> returned address is %1 <<<").arg( getAddress() ));
	/// FINE FIXTHERMO

	SetIcons((char *)icon_names.at(0)->ascii(), (char *)icon_names.at(1)->ascii(),
		(char *)icon_names.at(4)->ascii(), (char *)icon_names.at(5)->ascii());
	strncpy(&manIco[0], icon_names.at(2)->ascii(), sizeof(manIco));
	strncpy(&autoIco[0], icon_names.at(3)->ascii(), sizeof(autoIco));
	connect(this,SIGNAL(dxClick()),this,SLOT(aumSetpoint()));
	connect(this,SIGNAL(sxClick()),this,SLOT(decSetpoint()));
	setChi("4");
	stato=device_status_thermr::S_MAN;
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_thermr_device( getAddress() );
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), this, SLOT(status_changed(QPtrList<device_status>)));
	delta_setpoint = 0;
	connect(&setpoint_timer, SIGNAL(timeout()), this, SLOT(sendSetpoint()));
	sxButton->setAutoRepeat(true);
	dxButton->setAutoRepeat(true);
}

void termoPage::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_stat(stat_var::STAT);
	stat_var curr_local(stat_var::LOCAL);
	stat_var curr_sp(stat_var::SP);
	stat_var curr_temp(stat_var::TEMPERATURE);
	stat_var curr_info_sonda(stat_var::INFO_SONDA);
	stat_var curr_info_centrale(stat_var::INFO_CENTRALE);
	openwebnet msg_open;
	int delta = 0;
	bool aggiorna = false;
	qDebug("termoPage::status_changed()");
	QPtrListIterator<device_status> *dsi = new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) 
	{
		switch (ds->get_type()) 
		{
			case device_status::THERMR:
				qDebug("Th. regulator status variation");
				ds->read(device_status_thermr::STAT_INDEX, curr_stat);
				ds->read(device_status_thermr::LOCAL_INDEX, curr_local);
				ds->read(device_status_thermr::SP_INDEX, curr_sp);
				ds->read((int)device_status_thermr::INFO_SONDA, curr_info_sonda);
				qDebug("info_sonda = %d", curr_info_sonda.get_val());
				ds->read((int)device_status_thermr::INFO_CENTRALE, curr_info_centrale);
				qDebug("info_centrale = %d", curr_info_centrale.get_val());
				if (delta_setpoint == 1)
				{
					delta = atoi(strstr(&setpoint[0],".")+1);
					delta+=atoi(&setpoint[0])*10;
					curr_sp.set_val(delta);
					ds->write_val((int)device_status_thermr::SP_INDEX, curr_sp);
					ds->read(device_status_thermr::SP_INDEX, curr_sp);
					delta_setpoint = 0;
				}
				qDebug("stat = %d", curr_stat.get_val());
				qDebug("loc = %d", curr_local.get_val());
				qDebug("sp = %d", curr_sp.get_val());
				if (curr_stat.initialized()) 
				{
					switch(curr_stat.get_val()) {
						case device_status_thermr::S_MAN:
							qDebug("stato S_MAN");
							stato = device_status_thermr::S_MAN;
							mostra(BUT1);
							mostra(BUT2);
							nascondi(ICON);
							tempImp->show();
							aggiorna = true;
							if (isShown()) 
							{
								((sottoMenu*)parentWidget())->setNavBarMode(4,&autoIco[0]);
								((sottoMenu*)parentWidget())->forceDraw();
							}
							break;
						case device_status_thermr::S_AUTO:
							qDebug("stato S_AUTO");
							stato = device_status_thermr::S_AUTO;
							nascondi(BUT1);
							nascondi(BUT2);
							nascondi(ICON);
							tempImp->show();
							aggiorna = true;
							if(isShown())
							{
								((sottoMenu*)parentWidget())->setNavBarMode(4,&manIco[0]);
								((sottoMenu*)parentWidget())->forceDraw();
							}
							break;
						case device_status_thermr::S_ANTIGELO:
						case device_status_thermr::S_TERM:
						case device_status_thermr::S_GEN:
							qDebug("stato S_TERM");
							nascondi(BUT1);
							nascondi(BUT2);
							mostra(ICON);
							tempImp->hide();
							impostaAttivo(1);
							aggiorna= true;
							stato = device_status_thermr::S_TERM;
							if(isShown())
							{
								((sottoMenu*)parentWidget())->setNavBarMode(3, (char *)"");
								((sottoMenu*)parentWidget())->forceDraw();
							}
							break;
						case device_status_thermr::S_OFF:
							qDebug("stato S_OFF");
							mostra(ICON);
							nascondi(BUT1);
							nascondi(BUT2);
							tempImp->hide();
							impostaAttivo(0);
							aggiorna= true;
							stato = device_status_thermr::S_OFF;
							if(isShown())
							{

								((sottoMenu*)parentWidget())->setNavBarMode(3, (char *)"");
								((sottoMenu*)parentWidget())->forceDraw();
							}
							break;
						default:
							qDebug("unk status");
							break;
					}
				}
				if(curr_local.initialized()) 
				{
					qDebug("loc = %d", curr_local.get_val());
					switch(curr_local.get_val()) 
					{
						case 0:
							val_imp=3;
							isOff=0;
							isAntigelo=0;
							break;
						case 1:
							val_imp=4;
							isOff=0;
							isAntigelo=0;
							break;
						case 2:	
							val_imp=5;
							isOff=0;
							isAntigelo=0;
							break;
						case 3:	
							val_imp=6;
							isOff=0;
							isAntigelo=0;
							break;
						case 11:	
							val_imp=2;
							isOff=0;
							isAntigelo=0;
							break;
						case 12:	
							val_imp=1;
							isOff=0;
							isAntigelo=0;
							break;
						case 13:	
							val_imp=0;
							isOff=0;
							isAntigelo=0;
							break;
						case 4:	
							val_imp=0;
							isOff=1;
							isAntigelo=0;
							break;
						case 5:	
							val_imp=0;
							isOff=0;
							isAntigelo=1;
							break;
					}
				}
				if((curr_sp.initialized()) && (delta_setpoint == 0))
				{
					//Set Point
					float icx;
					char	tmp[10];   
					icx = curr_sp.get_val();
					qDebug("temperatura setpoint: %d",(int)icx);
					memset(setpoint,'\000',sizeof(setpoint));
					if (icx>=1000)
					{
						strcat(setpoint,"-");
						icx=icx-1000;
					}
					icx/=10;
					sprintf(tmp,"%.1f",icx);
					strcat(setpoint,tmp);
					strcat(setpoint,"\272C");
					aggiorna=1;
					break;
				}
				break;
			case device_status::TEMPERATURE_PROBE:
				ds->read(device_status_temperature_probe::TEMPERATURE_INDEX, curr_temp);
				qDebug("temperature probe status variation");
				//Temperatura misurata
				float icx;
				char	tmp[10];

				icx = curr_temp.get_val();
				qDebug("temperatura misurata: %d",(int)icx);
				memset(temp,'\000',sizeof(temp));
				if (icx>=1000)
				{
					strcat(temp,"-");
					icx=icx-1000;
				}
				icx/=10;
				sprintf(tmp,"%.1f",icx);
				qDebug("-1: tmp: %.1f - %s",icx, &tmp[0]);
				strcat(temp,tmp);
				qDebug("-1: temp: %s", &temp[0]);
				strcat(temp,"\272C");
				qDebug("-2: temp: %s", &temp[0]);
				aggiorna=1;
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}

char* termoPage::getChi()
{
	return("4");
}

char* termoPage::getAutoIcon()
{
	return &autoIco[0];
}

char* termoPage::getManIcon()
{
	return &manIco[0];
}

void termoPage::autoMan()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));


	if    (stato==device_status_thermr::S_MAN)
	{
		strcat(pippo,"*4*311*#");
		strcat(pippo,getAddress());
	}
	else 
	{
		int icx;

		icx=atoi(strstr(&setpoint[0],".")+1);
		if(setpoint[0]=='-')
			icx=-icx;
		icx+=atoi(&setpoint[0])*10;
		if (icx<0)
			icx=(abs(icx))+1000;
		strcat(pippo,"*#4*#");
		strcat(pippo,getAddress());
		strcat(pippo,"*#14*");
		sprintf(pippo,"%s%04d",pippo,icx);
		strcat(pippo,"*3");
	}

	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);
}

void termoPage::hide()
{
	disconnect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));
	banner::hide();
}

void termoPage::show()
{
	disconnect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));    /*mi assicuro che ci sia un solo collegamento*/
	connect(parentWidget(),SIGNAL(goDx()), this, SLOT(autoMan()));
	banner::show();
}



void termoPage::aumSetpoint()
{
	openwebnet msg_open;
	char    pippo[50];
	int 	icx;

	memset(pippo,'\000',sizeof(pippo));
	icx=atoi(strstr(&setpoint[0],".")+1);
	icx+=atoi(&setpoint[0])*10;
	if (icx>395)
		return;
	else
		icx=(abs(icx))+5;
	SetSetpoint(icx);
	setpoint_timer.start(2000);
	delta_setpoint = 1;
}

void termoPage::decSetpoint()
{
	openwebnet msg_open;
	char    pippo[50];
	int 	icx;

	memset(pippo,'\000',sizeof(pippo));
	icx=atoi(strstr(&setpoint[0],".")+1);
	icx+=atoi(&setpoint[0])*10;
	if (icx<35)
		return;
	else
		icx=(abs(icx))-5;
	SetSetpoint(icx);
	setpoint_timer.start(2000);
	delta_setpoint = 1;
}

void termoPage::SetSetpoint(float icx)
{
	char tmp[10];

	qDebug("NEW temperatura setpoint: %d",(int)icx);
	memset(setpoint,'\000',sizeof(setpoint));
	if (icx>=1000)
	{
		strcat(setpoint,"-");
		icx=icx-1000;
	}
	icx/=10;
	sprintf(tmp,"%.1f",icx);
	strcat(setpoint,tmp);
	strcat(setpoint,"\272C");
	Draw();
}

void termoPage::sendSetpoint()
{
	openwebnet msg_open;
	char pippo[50];
	int icx;

	qDebug("termoPage::sendSetpoint()");
	memset(pippo,'\000',sizeof(pippo));
	icx=atoi(strstr(&setpoint[0],".")+1);
	icx+=atoi(&setpoint[0])*10;
	memset(pippo,'\000',sizeof(pippo));
	/// FRAME VERSO LA CENTRALE
	strcat(pippo,"*#4*#");
	strcat(pippo,getAddress());
	strcat(pippo,"*#14*");
	sprintf(pippo,"%s%04d",&pippo[0],icx);
	strcat(pippo,"*3##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);
	setpoint_timer.stop();
}


void termoPage::inizializza()
{ 
	// See frame_interpreter.cpp
}


/*****************************************************************
 **zonaAnti
 ****************************************************************/

void zonaAnti::setIcons()
{
	if(isActive()) {
		banner::SetIcons((unsigned char)1, sparzIName);
		banner::SetIcons((unsigned char)3, zonaAttiva);
	} else {
		banner::SetIcons((unsigned char)1, parzIName);
		banner::SetIcons((unsigned char)3, zonaNonAttiva);
	}
}


	zonaAnti::zonaAnti( QWidget *parent,const char *name,char* indirizzo,char* iconzona, char* IconDisactive, char *IconActive, char *iconSparz, /*char *icon ,char *pressedIcon ,*/int period,int number )
: bannOnIcons ( parent, name )
{       
	char    pippo[MAX_PATH];
	char    pluto[MAX_PATH];



	setAddress(indirizzo);
	qDebug("zonaAnti::zonaAnti()");
	// Mail agresta 22/06
	parzIName = IMG_PATH "btnparzializza.png";
	sparzIName = IMG_PATH "btnsparzializza.png";
	getZoneName(iconzona, &pippo[0], indirizzo, sizeof(pippo));
	//getZoneName(IconDisactive, &pluto[0], indirizzo, sizeof(pluto));
	qDebug("icons %s %s %s", pippo, parzIName, sparzIName);
	zonaAnti::SetIcons(sparzIName, &pippo[0], IconDisactive);
	if (BannerText) {
		BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
		BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
		BannerText->setText(name);
	}
	zonaAttiva = IconActive;
	zonaNonAttiva = IconDisactive;
	setChi("5");
	already_changed = false;
	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_zonanti_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));    
	// Button will be disabled later
	//nascondi(BUT2);
	//nascondi(BUT1);
	abilitaParz(true);
	setIcons();
	zonaAnti::Draw();
}

void zonaAnti::SetIcons(char *i1, char *i2, char *i3)
{
	banner::SetIcons((unsigned char)1, i1);
	banner::SetIcons((unsigned char)2, i2);
	banner::SetIcons((unsigned char)3, i3);
}

void zonaAnti::Draw()
{
	qDebug("sxButton = %p", sxButton);
	sxButton->setPixmap(*Icon[1]);
	if (pressIcon[1])
		sxButton->setPressedPixmap(*pressIcon[1]);
	BannerIcon->setPixmap(*Icon[2]);
	BannerIcon2->setPixmap(*Icon[3]);
}

int zonaAnti::getIndex(void)
{
	return atoi(&(getAddress()[1]));
}

#if 0
void zonaAnti::gestFrame(char* frame)
{    
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);


	if (!strcmp(msg_open.Extract_chi(),"5"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),getAddress()) )   )   
		{
			if (!strcmp(msg_open.Extract_cosa(),"11")) 		
			{
				if (!isActive())
				{
					impostaAttivo(1);
					aggiorna=1;
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"18")) 
			{
				if (isActive())
				{
					impostaAttivo(0);
					aggiorna=1;
				}
			}
		}
	}    
	if (aggiorna) {
		setIcons();
		Draw();
	}
}
#else
void zonaAnti::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("zonAnti::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		int s;
		switch (ds->get_type()) {
			case device_status::ZONANTI:
				qDebug("Zon.anti status variation");
				ds->read(device_status_zonanti::ON_OFF_INDEX, curr_status);
				s = curr_status.get_val();
				qDebug("stat is %d", s);
				if(!isActive() && s) {
					impostaAttivo(1);
					qDebug("new status = %d", s);
					aggiorna = true;
				} else if(isActive() && !s) {
					impostaAttivo(0);
					qDebug("new status = %d", s);
					aggiorna = true;
				}
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna) {
		if(!already_changed) {
			already_changed = true;
			emit(partChanged(this));
		}
		setIcons();
		Draw();
	}
	delete dsi;
}
#endif


char* zonaAnti::getChi()
{
	return("5");
}

void zonaAnti::ToggleParzializza()
{
	qDebug("zonaAnti::ToggleParzializza()");
	impostaAttivo(!isActive());
	if(!already_changed) {
		already_changed = true;
		emit(partChanged(this));
	}
	setIcons();
	Draw();
}

void zonaAnti::abilitaParz(bool ab)
{
	qDebug("zonaAnti::abilitaParz(%d)", ab);
	if(ab) {
		connect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		mostra(BUT1);
	} else {
		disconnect(this, SIGNAL(sxClick()), this, SLOT(ToggleParzializza()));
		nascondi(BUT1);
	}
	Draw();
}

void zonaAnti::clearChanged()
{
	already_changed = false;
}


void zonaAnti::inizializza()
{ 
	openwebnet msg_open;
	char    pippo[50];
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#5*");
	strcat(pippo,getAddress());
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendInit(msg_open.frame_open);    
}


/*****************************************************************
 **impAnti
 ****************************************************************/


	impAnti::impAnti( QWidget *parent,const char *name,char* indirizzo,char* IconOn, char* IconOff, char* IconInfo, char* IconActive, int ,int  )
:  bann2butLab( parent, name )
{      
	char pippo[MAX_PATH];

	memset(pippo,'\000',sizeof(pippo));
	tasti = NULL;
	if (IconActive)
		strncpy(&pippo[0],IconActive,strstr(IconActive,".")-IconActive-3);

	strcat(pippo,"dis");
	strcat(pippo,strstr(IconActive,"."));
	SetIcons(  IconInfo,IconOff,&pippo[0],IconOn,IconActive);
	setChi("5");
	send_part_msg = false;
	inserting = false;
	memset(le_zone, 0, sizeof(le_zone));
	nascondi(BUT2);
#if 0
	// orig
	impostaAttivo(2);
#else
	impostaAttivo(0);
	Draw();
#endif
	// BUT2 and 4 are actually both on the left of the banner.
#if 0
	// orig
	connect(this,SIGNAL(dxClick()),this,SLOT(Inserisci()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Disinserisci()));               
	//     connect(this,SIGNAL(sxClick()),this,SIGNAL(CodaAllarmi()));                
#else
	connect(this,SIGNAL(dxClick()),this,SLOT(Disinserisci()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Inserisci()));  
#endif
	connect(this,SIGNAL(sxClick()), parentWidget(),SIGNAL(goDx()));

	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_impanti_device();
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));

}

#if 0
void impAnti::gestFrame(char* frame)
{    
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);


	if (!strcmp(msg_open.Extract_chi(),"5"))
	{
		if  (! strcmp(msg_open.Extract_dove(),"0" )   )   
		{
			if (!strcmp(msg_open.Extract_cosa(),"8")) 		
			{
				if (!isActive())
				{
					impostaAttivo(2);
					nascondi(BUT4);
					nascondi(BUT1);
					mostra(BUT2);
					aggiorna=1;
					qDebug("IMPIANTO INSERITO !!");
					emit(impiantoInserito());
					emit(abilitaParz(false));
				}
			}
			else if (!strcmp(msg_open.Extract_cosa(),"9")) 
			{
				if (isActive())
				{
					impostaAttivo(0);
					nascondi(BUT2);
					mostra(BUT4);		    
					aggiorna=1;
					qDebug("IMPIANTO DISINSERITO !!");
					emit(abilitaParz(true));
					emit(clearChanged());
					send_part_msg = false;
				}
			}
		}
	}    
	if (aggiorna)
		Draw();
}
#else
void impAnti::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::ON_OFF);
	bool aggiorna = false;
	qDebug("impAnti::status_changed()");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		int s;
		switch (ds->get_type()) {
			case device_status::IMPANTI:
				qDebug("Imp.anti status variation");
				ds->read(device_status_impanti::ON_OFF_INDEX, curr_status);
				s = curr_status.get_val();
				qDebug("status = %d", s);
				if(!isActive() && s) {
					impostaAttivo(2);
					nascondi(BUT4);
					nascondi(BUT1);
					mostra(BUT2);
					aggiorna=1;
					qDebug("IMPIANTO INSERITO !!");
					emit(impiantoInserito());
					emit(abilitaParz(false));
					connect(&insert_timer, SIGNAL(timeout()), this, SLOT(inizializza()));
					insert_timer.start(5000);
					send_part_msg = false;
				} else if(isActive() && !s) {
					impostaAttivo(0);
					nascondi(BUT2);
					mostra(BUT4);		    
					aggiorna=1;
					qDebug("IMPIANTO DISINSERITO !!");
					emit(abilitaParz(true));
					emit(clearChanged());
					send_part_msg = false;		
				}
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if(aggiorna)
		Draw();
	delete dsi;
}
#endif

int impAnti::getIsActive(int zona)
{
	if(le_zone[zona] !=  NULL)
		return le_zone[zona]->isActive();
	else
		return -1;
}

void impAnti::ToSendParz(bool s)
{
	send_part_msg = s;
}

void impAnti::Inserisci()
{
	qDebug("impAnti::Inserisci()");
	int s[MAX_ZONE];
	for(int i=0; i<MAX_ZONE; i++) 
	{
		if(le_zone[i] !=  NULL)
			s[i] = le_zone[i]->isActive();
		else
			s[i] = -1;
	}
	if(tasti)
		delete tasti;
	inserting = true;
	tasti = new tastiera_con_stati(s, NULL, "");
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(Insert1(char*)));
	tasti->setBGColor(backgroundColor());
	tasti->setFGColor(foregroundColor());
	tasti->setMode(tastiera::HIDDEN);
	tasti->showTastiera();
	//parentWidget()->hide();
	//    this->hide();
}
void impAnti::Disinserisci()
{
	if(tasti)
		delete tasti;
	tasti = new tastiera(NULL, "");
	connect(tasti, SIGNAL(Closed(char*)), this, SLOT(DeInsert(char*)));
	tasti->setBGColor(backgroundColor());
	tasti->setFGColor(foregroundColor());    
	tasti->setMode(tastiera::HIDDEN);
	tasti->showTastiera();
	//parentWidget()->hide();

	//    this->hide();    
}


void impAnti::Insert1(char* pwd)
{  
	openwebnet msg_open;
	char    pippo[50];

	if (!pwd) 
		goto end;
	passwd = pwd;

	qDebug("impAnti::Insert()");
	if(!send_part_msg) {
		Insert3();
		goto end;
	}
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*50#");
	strcat(pippo,pwd);
	strcat(pippo,"#");
	for(int i=0; i<MAX_ZONE; i++)
		strcat(pippo, le_zone[i] && le_zone[i]->isActive() ? 
				"0" : "1");
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	qDebug("sending part frame %s", pippo);
	emit sendFrame(msg_open.frame_open);
	send_part_msg = false;
	part_msg_sent = true;
end:
	parentWidget()->show();
}

void impAnti::Insert2()
{
	qDebug("impAnti::Insert2()");
	if(!inserting)
		return;
	// 5 seconds between first open ack and open insert messages
	connect(&insert_timer, SIGNAL(timeout()), this, SLOT(Insert3()));
	// single shot timer
	insert_timer.start(6000, TRUE);
}

void impAnti::Insert3()
{
	qDebug("impAnti::Insert3()");
	char *pwd = passwd;
	openwebnet msg_open;
	char    pippo[50];
	emit(clearAlarms());
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*5*36#");
	strcat(pippo,pwd);
	strcat(pippo,"*0##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
	parentWidget()->show();       
	inserting = false;
	disconnect(&insert_timer, SIGNAL(timeout()), this, SLOT(Insert3()));
	connect(&insert_timer, SIGNAL(timeout()), this, SLOT(inizializza()));
	insert_timer.start(5000);
}

void impAnti::DeInsert(char* pwd)
{
	qDebug("impAnti::DeInsert()");
	if (pwd)
	{
		openwebnet msg_open;
		char    pippo[50];

		memset(pippo,'\000',sizeof(pippo));
		strcat(pippo,"*5*36#");
		strcat(pippo,pwd);
		strcat(pippo,"*0##");
		msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
		emit sendFrame(msg_open.frame_open);    
	}    
	parentWidget()->show();   
	//    qDebug("disinserisco con %s", pwd);
}

void impAnti::openAckRx()
{
	qDebug("impAnti::openAckRx()");
	if(!inserting) {
		qDebug("Not inserting");
		return;
	}
	if(!part_msg_sent) return;
	part_msg_sent = false;
	qDebug("waiting 5 seconds before sending insert message");
	// Do second step of insert
	Insert2();
}

void impAnti::openNakRx()
{
	qDebug("impAnti::openNakRx()");
	if(!part_msg_sent) return;
	//Agre - per sicurezza provo a continuare per evitare di non inserire l'impianto
	openAckRx();
	part_msg_sent = false;
}

char* impAnti::getChi()
{
	return("5");
}

void impAnti::setZona(zonaAnti *za)
{
	int index = za->getIndex() - 1;
	if((index >= 0) && (index < MAX_ZONE))
		le_zone[index] = za;
}

void impAnti::partChanged(zonaAnti *za)
{
	qDebug("impAnti::partChanged");
	send_part_msg = true;
}

void impAnti::inizializza()
{
	qDebug("impAnti::inizializza()");
	insert_timer.stop();
	disconnect(&insert_timer, SIGNAL(timeout()), this, SLOT(inizializza()));
	emit sendInit("*#5*0##");    
}

void impAnti::hide()
{
	qDebug("impAnti::hide()");
	banner::hide();
	if(tasti && tasti->isShown()) {
		qDebug("HIDING KEYBOARD");
		tasti->hide();
	}
}


/*****************************************************************
 **gesModScen
 ****************************************************************/

	gesModScen::gesModScen( QWidget *parent, const char *name ,char*indirizzo,char* IcoSx,char* IcoDx,char* IcoCsx,char* IcoCdx,char* IcoDes, char* IcoSx2, char* IcoDx2)
:  bann4tasLab(parent, name)
{
	strcpy(&iconOn[0],IcoSx);
	strcpy(&iconStop[0],IcoSx2);       
	strcpy(&iconInfo[0],IcoDx);
	strcpy(&iconNoInfo[0],IcoDx2);       

	SetIcons (IcoSx, IcoDx, IcoCsx, IcoCdx, IcoDes);
	nascondi(BUT3);
	nascondi(BUT4);    

	memset(&cosa[0],'\000',sizeof(&cosa[0]) );
	memset(&dove[0],'\000',sizeof(&dove[0]) );    
	//   strcpy(&cosa[0], indirizzo);

	//   *strstr(&cosa[0],"*")='\000';
	qDebug("gesModScen::gesModScen(): indirizzo = %s", indirizzo);
	if (strstr(indirizzo,"*"))
	{
		strncpy(&cosa[0], indirizzo, strstr(indirizzo,"*")-indirizzo);

		strcpy(&dove[0], strstr(indirizzo,"*")+1);    
	}
	sendInProgr=0;

	setAddress(dove);
	impostaAttivo(2);   
	connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
	connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	connect(this,SIGNAL(csxClick()),this,SLOT(startProgScen()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(cancScen()));    

	// Crea o preleva il dispositivo dalla cache
	dev = btouch_device_cache.get_modscen_device(getAddress());
	// Get status changed events back
	connect(dev, SIGNAL(status_changed(QPtrList<device_status>)), 
			this, SLOT(status_changed(QPtrList<device_status>)));
	// Will be initialized later
	//dev->init();

}

void gesModScen::attivaScenario()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*0*");
	strcat(pippo,&cosa[0]);
	strcat(pippo,"*");
	strcat(pippo,&dove[0]);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
}
void gesModScen::enterInfo()
{
	nascondi(ICON);
	mostra(BUT4);    
	mostra(BUT3);      
	SetIcons(uchar(1),&iconNoInfo[0]);
	qDebug(&iconNoInfo[0]);
	disconnect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	connect(this,SIGNAL(dxClick()),this,SLOT(exitInfo())); 
	Draw();
}
void gesModScen::exitInfo()
{
	mostra(ICON);
	nascondi(BUT4);    
	nascondi(BUT3);      
	SetIcons(uchar(1),&iconInfo[0]);
	qDebug(&iconInfo[0]);
	connect(this,SIGNAL(dxClick()),this,SLOT(enterInfo()));
	disconnect(this,SIGNAL(dxClick()),this,SLOT(exitInfo()));     
	Draw();
}
void gesModScen::startProgScen()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*0*40#");
	strcat(pippo,&cosa[0]);
	strcat(pippo,"*");
	strcat(pippo,&dove[0]);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
	sendInProgr=1;
}
void gesModScen::stopProgScen()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*0*41#");
	strcat(pippo,&cosa[0]);
	strcat(pippo,"*");
	strcat(pippo,&dove[0]);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
	sendInProgr=0;
}
void gesModScen::cancScen()
{
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*0*42#");
	strcat(pippo,&cosa[0]);
	strcat(pippo,"*");
	strcat(pippo,&dove[0]);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    
}

#if 0
void gesModScen::gestFrame(char* frame)
{
	openwebnet msg_open;
	char aggiorna;

	aggiorna=0;

	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);


	if (!strcmp(msg_open.Extract_chi(),"0"))
	{
		if ( (! strcmp(msg_open.Extract_dove(),&dove[0])) )
		{
			if (!strncmp(msg_open.Extract_cosa(),"40",2)) 		
			{
				if (sendInProgr)
				{	
					SetIcons(uchar(0),&iconStop[0]);       
					disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
					connect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));

					in_progr=0;
				}
				else
				{
					//   nascondi(BUT2);
					in_progr=1;
					exitInfo();
				}
				aggiorna=1;
			}
			else if  (!strncmp(msg_open.Extract_cosa(),"43",2))
			{
				bloccato=1;
				exitInfo();
				aggiorna=1;
			}
			else if  (!strncmp(msg_open.Extract_cosa(),"45",2))
			{
				in_progr=1;
				exitInfo();
				aggiorna=1;
			}
			else if  (!strncmp(msg_open.Extract_cosa(),"41",2))  
			{
				SetIcons(uchar(0),&iconOn[0]);       
				disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
				//mostra(BUT2);
				aggiorna=1;
				in_progr=0;
			}
			else if  (!strncmp(msg_open.Extract_cosa(),"44",2))
			{
				SetIcons(uchar(0),&iconOn[0]);       
				disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
				disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
				//mostra(BUT2);
				aggiorna=1;
				bloccato=0;
			} 
			else if (!strcmp(msg_open.Extract_cosa(),"0")) 
			{
			}
		}
	}    
	/*  if (!strcmp(msg_open.Extract_chi(),"1001"))
	    {
	    if ( (! strcmp(msg_open.Extract_dove(),&dove[0])) )
	    {	
	    if ( (! strcmp(msg_open.Extract_grandezza(),"1")) )
	    {
	    if ( (! strcmp(msg_open.Extract_valori(0),"55")) )
	    mostra(BUT2);	
	    }
	    }
	    }*/

	if (aggiorna)
	{
		//        qDebug("bloccato= %d * in_progr= %d",bloccato,in_progr);
		if (bloccato || in_progr)
			nascondi(BUT2);
		else
			mostra(BUT2);
		Draw();
	}
}
#else
void gesModScen::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_status(stat_var::STAT);
	qDebug("gesModScen::status_changed");
	QPtrListIterator<device_status> *dsi = 
		new QPtrListIterator<device_status>(sl);
	bool aggiorna = false;
	dsi->toFirst();
	device_status *ds;
	while( ( ds = dsi->current() ) != 0) {
		switch (ds->get_type()) {
			case device_status::MODSCEN:
				qDebug("Modscen status change");
				ds->read((int)device_status_modscen::STAT_INDEX, curr_status);
				switch(curr_status.get_val()) {
					case device_status_modscen::PROGRAMMING_START:
						qDebug("Programming start");
						if (sendInProgr) {	
							SetIcons(uchar(0),&iconStop[0]);       
							disconnect(this,SIGNAL(sxClick()), this,
									SLOT(attivaScenario()));
							connect(this,SIGNAL(sxClick()), this,
									SLOT(stopProgScen()));
							in_progr=0;
						} else {
							in_progr=1;
							exitInfo();
						}
						aggiorna=1;
						break;
					case device_status_modscen::LOCKED:
						qDebug("Locked");
						bloccato=1;
						exitInfo();
						aggiorna=1;
						break;
					case device_status_modscen::BUSY:
						qDebug("Busy");
						in_progr=1;
						exitInfo();
						aggiorna=1;
						break;
					case device_status_modscen::PROGRAMMING_STOP:
						qDebug("Programming stop");
						SetIcons(uchar(0),&iconOn[0]);       
						disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
						connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
						disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
						aggiorna=1;
						in_progr=0;
						break;
					case device_status_modscen::UNLOCKED:
						qDebug("Unlocked");
						SetIcons(uchar(0),&iconOn[0]);       
						disconnect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
						connect(this,SIGNAL(sxClick()),this,SLOT(attivaScenario()));
						disconnect(this,SIGNAL(sxClick()),this,SLOT(stopProgScen()));
						aggiorna=1;
						bloccato=0;
						break;
					default:
						qDebug("Unknown status %d", curr_status.get_val());
				}
				break;
			default:
				qDebug("device status of unknown type (%d)", ds->get_type());
				break;
		}
		++(*dsi);
	}
	if (aggiorna) {
		//        qDebug("bloccato= %d * in_progr= %d",bloccato,in_progr);
		if (bloccato || in_progr)
			nascondi(BUT2);
		else
			mostra(BUT2);
		Draw();
	}
	delete dsi;
}
#endif

void gesModScen::inizializza()
{
#if 0   
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#0*");
	strcat(pippo,&dove[0]);
	strcat(pippo,"##");
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	emit sendFrame(msg_open.frame_open);    	
#endif
	nascondi(BUT2);     
}

/*****************************************************************
 ** Scenario evoluto
 ****************************************************************/	

int scenEvo::next_serial_number = 1;

scenEvo::scenEvo( QWidget *parent, const char *name, 
		QPtrList<scenEvo_cond> *c, 
		char *i1, char *i2, char *i3, char *i4, char *i5, 
		char *i6, char *i7, QString act, int enable)
: bann3But( parent, name )
{
	if(c)
		condList = new QPtrList<scenEvo_cond>(*c);
	cond_iterator = new QPtrListIterator<scenEvo_cond>(*condList);
	cond_iterator->toFirst();
	scenEvo_cond *co;
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	serial_number = next_serial_number++;
	while( ( co = ci->current() ) != 0) {
		qDebug(co->getDescription());
		co->set_serial_number(serial_number);
		qDebug("connecting richStato and frame_available signals");
		connect(co, SIGNAL(verificata()), this, SLOT(trig()));
		connect(co, SIGNAL(okAll()), this, SLOT(saveAndApplyAll()));
		connect(co, SIGNAL(resetAll()), this, SLOT(resetAll()));
		++(*ci);
	}
	delete ci;
	action = act;
	SetIcons(i1, i2 , i3, i4);
	impostaAttivo(enable);
	connect(this,SIGNAL(sxClick()),this,SLOT(toggleAttivaScev()));
	connect(this,SIGNAL(dxClick()),this,SLOT(configScev()));
	connect(this,SIGNAL(centerClick()),this,SLOT(forzaScev()));
	connect(parent, SIGNAL(frez(bool)), this, SLOT(freezed(bool)));    
}

void scenEvo::toggleAttivaScev(void)
{
	qDebug("scenEvo::toggleAttivaScev");
	impostaAttivo(!isActive());
	Draw();
	const char *s = isActive() ? "1" : "0";
	copyFile("cfg/conf.xml","cfg/conf1.lmx");
	setCfgValue("cfg/conf1.lmx", SCENARIO_EVOLUTO, "enable", s, serial_number);
	QDir::current().rename("cfg/conf1.lmx","cfg/conf.xml",FALSE);
}


void scenEvo::configScev(void)
{
	qDebug("scenEvo::configScev");
	hide();
	scenEvo_cond *co = cond_iterator->current();
	qDebug("Invoco %p->mostra()", co);
	co->setBGColor(backgroundColor());
	co->setFGColor(foregroundColor());
	connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	//connect(co, SIGNAL(SaveCondition()), this, SLOT(saveCond()));
	co->mostra();
}

void scenEvo::forzaScev(void)
{
	qDebug("scenEvo::forzaScev");
	// Forced trigger
	trig(true);
	Draw();
}

void scenEvo::nextCond(void)
{
	qDebug("scenEvo::nextCond()");
	scenEvo_cond *co = cond_iterator->current();
	disconnect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->hide();
	if(!cond_iterator->atLast()) {
		++(*cond_iterator);
		co = cond_iterator->current();
		qDebug("co = %p", co);
		if(co) {
			co->setBGColor(backgroundColor());
			co->setFGColor(foregroundColor());
			connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			co->inizializza();
			co->mostra();
		}
	} else {
		cond_iterator->toFirst();
		Draw();
		show();
	}
}

void scenEvo::prevCond(void)
{
	qDebug("scenEvo::prevCond()");
	scenEvo_cond *co = cond_iterator->current();
	disconnect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
	disconnect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->hide();
	if(!cond_iterator->atFirst()) {
		--(*cond_iterator);
		co = cond_iterator->current();
		qDebug("co = %p", co);
		if(co) {
			co->setBGColor(backgroundColor());
			co->setFGColor(foregroundColor());
			connect(co, SIGNAL(SwitchToNext()), this, SLOT(nextCond()));
			connect(co, SIGNAL(SwitchToPrev()), this, SLOT(prevCond()));
			connect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
			co->mostra();
		}
	} else {
		Draw();
		show();
	}
}

void scenEvo::firstCond(void)
{
	qDebug("scenEvo::firstCond()");
	scenEvo_cond *co = cond_iterator->current();
	disconnect(co, SIGNAL(SwitchToFirst()), this, SLOT(firstCond()));
	co->hide();
	cond_iterator->toFirst();
	Draw();
	show();
}

void scenEvo::saveAndApplyAll(void)
{
	qDebug("scenEvo::saveAndApplyAll()");
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	scenEvo_cond *co;
	while( ( co = ci->current() ) != 0) {
		co->Apply();
		co->save();
		++(*ci);
	}
	delete ci;
}

void scenEvo::resetAll(void)
{
	qDebug("scenEvo::resetAll()");
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	scenEvo_cond *co;
	while( ( co = ci->current() ) != 0) {
		co->reset();
		++(*ci);
	}
	delete ci;
}

void scenEvo::Draw()
{
	// Icon[0] => left button (inactive)
	// pressIcon[0] => pressed left button (inactive)
	// Icon[1] => left button (active)
	// pressIcon[1] => pressed left button (active)
	// Icon[3] => center button
	// pressIcon[3] => pressed center button
	// Icon[2] => right button
	// pressIcon[2] => pressed right button
	qDebug("scenEvo::Draw(%p)", this);
	cond_iterator->toFirst();
	if ( (sxButton) && (Icon[0]) && (Icon[1])) {
		int sxb_index = isActive() ? 0 : 1;
		sxButton->setPixmap(*Icon[sxb_index]);
		if (pressIcon[sxb_index])
			sxButton->setPressedPixmap(*pressIcon[sxb_index]);
	}
	if ( (dxButton) && (Icon[2]) ) {
		dxButton->setPixmap(*Icon[2]);
		if(pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if ( (csxButton) && (Icon[3]) ) {
		csxButton->setPixmap(*Icon[3]);
		if(pressIcon[3])
			csxButton->setPressedPixmap(*pressIcon[3]);
	}
	if (BannerText) {
		BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
		BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
		BannerText->setText(testo);
		//     qDebug("TESTO: %s", testo);
	}
	if (SecondaryText) {	
		SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
		SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
		SecondaryText->setText(testoSecondario);
	}
}

const char *scenEvo::getAction() 
{
	return action.ascii() ; 
}

void scenEvo::setAction(const char *a)
{
	action = a ;
}


void scenEvo::trig(bool forced)
{
	if(!action) {
		qDebug("scenEvo::trig(), act = NULL, non faccio niente");
		return;
	}
	QPtrListIterator<scenEvo_cond> *ci;
	if(forced) 
		goto do_send;
	if(!isActive()) {
		qDebug("scenEvo::trig(), non abilitato, non faccio niente");
		return;
	}
	// Verifica tutte le condizioni
	ci = new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	scenEvo_cond *co;
	while( ( co = ci->current() ) != 0) {
		if(!co->isTrue()) {
			qDebug("Condizione %p (%s), non verificata, non faccio niente",
					co, co->getDescription());
			return;
		}
		++(*ci);
	}
	delete ci;
do_send:
	qDebug("scenEvo::trig(), act = %s", action.ascii());
	openwebnet msg_open;
	msg_open.CreateMsgOpen((char *)action.ascii(), action.length());
	emit sendFrame(msg_open.frame_open);    	
}

void scenEvo::freezed(bool f)
{
	qDebug("scenEvo::freezed(bool f)");
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	scenEvo_cond *co ;
	while( ( co = ci->current() ) != 0) {
		co->setEnabled(!f);
		++(*ci);
	}
	delete ci;
}

void scenEvo::gestFrame(char* frame)
{
	qDebug("scenEvo::gestFrame()");
#if 1
#if 0
	// devices talk directly to comm clients
	emit(frame_available(frame));
#endif
#else
	scenEvo_cond *co;
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	while( ( co = ci->current() ) != 0) {
		co->gestFrame(frame);
		++(*ci);
	}
	delete ci;
#endif
}

void scenEvo::inizializza(void)
{
	qDebug("scenEvo::inizializza()");
	scenEvo_cond *co;
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	while( ( co = ci->current() ) != 0) {
		co->inizializza();
		++(*ci);
	}
	delete ci;
}

void scenEvo::hide()
{
	qDebug("scenEvo::hide()");
	scenEvo_cond *co;
	QPtrListIterator<scenEvo_cond> *ci = 
		new QPtrListIterator<scenEvo_cond>(*condList);
	ci->toFirst();
	while( ( co = ci->current() ) != 0) {
		co->hide();
		++(*ci);
	}
	delete ci;
	banner::hide();
}

// FIXME: FAI IL DISTRUTTORE !!!!!


/*****************************************************************
 ** Scenario schedulato
 ****************************************************************/	
scenSched::scenSched(QWidget *parent, const char *name, char* Icona1,char *Icona2, char *Icona3, char* Icona4, char *aen, char *adis, char *astart, char *astop) : bann4But( parent, name )
{
	char pressIconName[100];
	action_enable = aen;
	action_disable = adis;
	action_start = astart;
	action_stop = astop;
	qDebug("scenSched::scenSched(), enable = %s, start = %s, stop = %s, "
			"disable = %s", aen, adis, astart, astop);
	qDebug("I1 = %s, I2 = %s, I3 = %s, I4 = %s", 
			Icona1, Icona2, Icona3, Icona4);
	SetIcons(Icona1, Icona3, Icona4, Icona2);
	if(aen[0]) {
		// sx
		qDebug("BUT1 attivo");
		connect(this, SIGNAL(sxClick()), this, SLOT(enable()));
	} else
		nascondi(BUT1);
	if(astart[0]) {
		// csx
		qDebug("BUT3 attivo");
		connect(this, SIGNAL(csxClick()), this, SLOT(start()));
	} else
		nascondi(BUT3);
	if(astop[0]) {
		// cdx
		qDebug("BUT4 attivo");
		connect(this, SIGNAL(cdxClick()), this, SLOT(stop()));
	} else
		nascondi(BUT4);
	if(adis[0]) {
		// dx
		qDebug("BUT2 attivo");
		connect(this, SIGNAL(dxClick()), this, SLOT(disable()));
	} else
		nascondi(BUT2);
	Draw();
}

void scenSched::enable(void)
{
	qDebug("scenSched::enable()");
	openwebnet msg_open;
	msg_open.CreateMsgOpen((char *)action_enable.ascii(), 
			action_enable.length());
	emit sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::start(void)
{
	qDebug("scenSched::start()");
	openwebnet msg_open;
	msg_open.CreateMsgOpen((char *)action_start.ascii(), 
			action_start.length());
	emit sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::stop(void)
{
	qDebug("scenSched::stop()");
	openwebnet msg_open;
	msg_open.CreateMsgOpen((char *)action_stop.ascii(), 
			action_stop.length());
	emit sendFrame(msg_open.frame_open);
	Draw();
}

void scenSched::disable(void)
{
	qDebug("scenSched::disable()");
	openwebnet msg_open;
	msg_open.CreateMsgOpen((char *)action_disable.ascii(), 
			action_disable.length());
	emit sendFrame(msg_open.frame_open);
	Draw();
}


void scenSched::Draw()
{
	// Icon[0] => left button
	// pressIcon[0] => pressed left button
	// Icon[1] => center left button 
	// pressIcon[1] => pressed center left button
	// Icon[3] => center right button
	// pressIcon[3] => pressed center right button
	// Icon[2] => right button
	// pressIcon[2] => pressed right button
	qDebug("scenSched::Draw()");
	if ((sxButton) && (Icon[0])) {
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}
	if ( (csxButton) && (Icon[1]) ) {
		csxButton->setPixmap(*Icon[1]);
		if(pressIcon[1])
			csxButton->setPressedPixmap(*pressIcon[1]);
	}
	if ( (cdxButton) && (Icon[3]) ) {
		cdxButton->setPixmap(*Icon[3]);
		if(pressIcon[3])
			cdxButton->setPressedPixmap(*pressIcon[3]);
	}
	if ( (dxButton) && (Icon[2]) ) {
		dxButton->setPixmap(*Icon[2]);
		if(pressIcon[2])
			dxButton->setPressedPixmap(*pressIcon[2]);
	}
	if (BannerText) {
		BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
		BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
		BannerText->setText(testo);
		//     qDebug("TESTO: %s", testo);
	}
#if 0
	if (SecondaryText) {	
		SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
		SecondaryText->setFont( QFont( "helvetica", 18, QFont::Bold ) );
		SecondaryText->setText(testoSecondario);
	}
#endif
}

/*****************************************************************
 ** Posto Esterno
 ****************************************************************/	

// Static pointer to call notifier manager
call_notifier_manager *postoExt::cnm = NULL;

// Static pointer to unknown station
call_notifier *postoExt::unknown_notifier = NULL;

postoExt::postoExt(QWidget *parent, const char *name, char* Icona1,char *Icona2, char *Icona3, char* Icona4, char *_where, char *_light, char *_key, char *_unknown, char* _txt1, char* _txt2, char* _txt3) : bann4tasLab( parent, name )
{
	char pressIconName[100];
	where = _where;
	descr = name;
	light = !strcmp(_light, "1");
	key = !strcmp(_key, "1");
	unknown = !strcmp(_unknown, "1");
	qDebug("postoExt::postoExt(), unknown = %s", _unknown);
	qDebug("I1 = %s, I2 = %s, I3 = %s, I4 = %s", 
			Icona1, Icona2, Icona3, Icona4);

	qDebug("light = %d, key = %d, unknown = %d", light, key, unknown);
	qDebug("descr = %s, where = %s", name, _where);
	qDebug("txt1 = %s, txt2 = %s, txt3 = %s", _txt1, _txt2, _txt3);
	//SetIcons(Icona1, Icona3, Icona4, Icona2);
	SetIcons (Icona2, Icona3, "", "", Icona1);
	if(key) {
		key_icon = Icona2;
		connect(this, SIGNAL(sxPressed()), this, SLOT(open_door_clicked()));
	} else
		nascondi(BUT1);
	if(light) {
		connect(this, SIGNAL(dxPressed()), this, SLOT(stairlight_pressed()));
		connect(this, SIGNAL(dxReleased()), this, SLOT(stairlight_released()));
		light_icon = Icona3;
	} else
		nascondi(BUT2);
	connect(parent, SIGNAL(frez(bool)), this, SIGNAL(freezed(bool))); 
	close_icon = Icona4;
	impostaAttivo(2);
	Draw();
	qDebug("creating call_notifier");
	call_notifier *cn = new call_notifier((QWidget *)NULL, 
			(char *)"call notifier", this, _txt1, _txt2, _txt3);
	qDebug("setting BG and FG colors");
	cn->setBGColor(backgroundColor());
	cn->setFGColor(foregroundColor());
	qDebug("cnm = %p", cnm);
	if(!cnm) {
		qDebug("Creating call notifier manager");
		cnm = new call_notifier_manager();
		connect(cnm, SIGNAL(frame_captured(call_notifier *)),
				this, SLOT(frame_captured_handler(call_notifier *)));
		connect(cnm, SIGNAL(call_notifier_closed(call_notifier *)),
				this, SLOT(call_notifier_closed(call_notifier *)));
		connect(btouch_device_cache.get_client_monitor(), 
				SIGNAL(frameIn(char *)),
				cnm, SLOT(gestFrame(char *)));
		connect(this, SIGNAL(freezed(bool)), cnm, SIGNAL(freezed(bool)));
	}
	cnm->add_call_notifier(cn);
	if(unknown && !unknown_notifier) {
		qDebug("Creating unknown station notifier");
		unknown_notifier = new call_notifier(NULL, "unk call notif", NULL, _txt1, _txt2, _txt3);
		cnm->set_unknown_call_notifier(unknown_notifier);
		unknown_notifier->setBGColor(backgroundColor());
		unknown_notifier->setFGColor(foregroundColor());
	}
}

void postoExt::gestFrame(char *s)
{
	qDebug("postoExt::gestFrame()");
}

void postoExt::frame_captured_handler(call_notifier *cn)
{
	qDebug("postoExt::frame_captured_handler()");
	// Just unfreeze
	//emit(svegl(1));
	emit(freeze(0));
}

void postoExt::call_notifier_closed(call_notifier *cn)
{
	qDebug("postoExt::call_notifier_closed()");
	//emit(svegl(0));
}

void postoExt::open_door_clicked(void)
{
	qDebug("postoExt::open_door_clicked()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();  
	char tmp[100];
	sprintf(tmp, "*6*10*%s##", where.ascii());
	msg_open.CreateMsgOpen(tmp, strlen(tmp));
	emit sendFrame(msg_open.frame_open);   
}

void postoExt::stairlight_pressed(void)
{
	qDebug("postoExt::stairlight_pressed()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();  
	char tmp[100];
	sprintf(tmp, "*6*12*%s##", where.ascii());
	msg_open.CreateMsgOpen(tmp, strlen(tmp));
	emit sendFrame(msg_open.frame_open);   
}

void postoExt::stairlight_released(void)
{
	qDebug("postoExt::stairlight_released()");
	openwebnet msg_open;
	msg_open.CreateNullMsgOpen();  
	char tmp[100];
	sprintf(tmp, "*6*11*%s##", where.ascii());
	msg_open.CreateMsgOpen(tmp, strlen(tmp));
	emit sendFrame(msg_open.frame_open);   
}

void postoExt::get_where(QString& out)
{
	out = where;
}

void postoExt::get_descr(QString& out)
{
	out = descr;
}

bool postoExt::get_light(void)
{
	return light;
}

bool postoExt::get_key(void)
{
	return key;
}

void postoExt::get_light_icon(QString& out)
{
	out = light_icon;
}

void postoExt::get_key_icon(QString& out)
{
	out = key_icon;
}

void postoExt::get_close_icon(QString& out)
{
	out = close_icon;
}

#if 0
void postoExt::freezed(bool f)
{


}
#endif


/*****************************************************************
 ** Ambiente diffusione sonora multicanale
 ****************************************************************/

ambDiffSon::ambDiffSon( QWidget *parent, const char *name, void *indirizzo, char* IconaSx, char* IconaDx, char *icon, QPtrList<dati_ampli_multi> *la, diffSonora *ds, sottoMenu *sorg, diffmulti *dm)
: bannBut2Icon( parent, name )
{
	qDebug("ambDiffSon::ambDiffSon() : %s %s %s %s", indirizzo, IconaSx, IconaDx, icon);
	char zoneIcon[50];
	getAmbName(IconaSx, zoneIcon, (char *)indirizzo, sizeof(zoneIcon));
	qDebug("zoneIcon = %s", zoneIcon);
	SetIcons(icon, zoneIcon, IconaDx);
	Draw();
	setAddress((char *)indirizzo);
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));

	//diffson = new diffSonora(NULL, "Diff sonora ambiente");
	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;
	QPtrListIterator<dati_ampli_multi> *lai = new QPtrListIterator<dati_ampli_multi>(*la);
	lai->toFirst();
	dati_ampli_multi *am;
	while( ( am = lai->current() ) != 0) 
	{
		QPtrList<QString> icons;
		QString qI1(am->I1);
		QString qI2(am->I2);
		QString qI3(am->I3);
		QString qI4(am->I4);
		icons.append(&qI1);
		icons.append(&qI2);
		icons.append(&qI3);
		icons.append(&qI4);

		if(am->tipo == AMPLIFICATORE)
		{
			qDebug("Adding amplifier (%d, %s %s)", am->tipo, (char *)am->indirizzo, am->descr->at(0)->ascii());
			diffson->addItem(am->tipo, (char *)am->descr->at(0)->ascii(), (char *)am->indirizzo, icons, am->modo);
		}
		else 
		{
			qDebug("Adding amplifier group");
			qDebug("indirizzo = %p", am->indirizzo);
			QPtrListIterator<QString> *lii = new QPtrListIterator<QString>(*(QPtrList<QString> *)(am->indirizzo));
			QString *i;
			lii->toFirst();
			while ( ( i = lii->current()) )
			{
				QStringList qsl = QStringList::split(QChar(','), *i);
				QPtrList<QString> *indirizzi = new QPtrList<QString>();
				indirizzi->setAutoDelete(true);
				for(int j=0; j<qsl.count(); j++)
					indirizzi->append(new QString(qsl[j]));
				diffson->addItem(am->tipo, (char *)am->descr->at(0)->ascii(), indirizzi, icons, am->modo);
				++(*lii);
			}
			delete lii;
		}
		++(*lai);
	}
	delete lai;
	setDraw(false);
}

void ambDiffSon::Draw()
{
	qDebug("ambDiffSon::Draw()");
	sxButton->setPixmap(*Icon[0]);
	if (pressIcon[0])
		sxButton->setPressedPixmap(*pressIcon[0]);
	BannerIcon->repaint();
	BannerIcon->setPixmap(*(Icon[1]));
	BannerIcon->repaint();
	BannerIcon2->repaint();
	BannerIcon2->setPixmap(*(Icon[3]));
	BannerIcon2->repaint();
	BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
	BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
	BannerText->setText(testo);
}

void ambDiffSon::hide()
{
	qDebug("ambDiffSon::hide()");
	setDraw(false);
	banner::hide();
}

void ambDiffSon::configura()
{
	qDebug("ambDiffSon::configura()");
	emit(ambChanged((char *)name(), false, getAddress()));
	qDebug("sorgenti->parent() = %p", sorgenti->parent());
	qDebug("disconnecting sorgenti->parent from diffmulti(%p)", diffmul);
	disconnect(sorgenti->parent(), SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	disconnect(sorgenti->parent(), SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	sorgenti->reparent(diffson, 0, diffson->geometry().topLeft());
	connect(diffson, SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	connect(diffson, SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	qDebug("connecting diffson(%p) to diffmul(%p)", diffson, diffmul);
	//connect(diffmul, SIGNAL(gestFrame(char *)), 
	//diffson, SIGNAL(gestFrame(char *)));
	//diffson->setGeom(0,0,MAX_WIDTH,MAX_HEIGHT);
	diffson->setFirstSource(actSrc);
	diffson->forceDraw();
	diffson->showFullScreen();
	setDraw(true);
}

void ambDiffSon::actSrcChanged(int a, int s)
{
	qDebug("ambDiffSon::actSrcChanged(%d, %d)", a, s);
	if(a != atoi(getAddress())) {
		qDebug("not my address, discarding event");
		return;
	}
	qDebug("First source's where is %d", actSrc);
	if(actSrc != (100 + a*10 + s))
	{
		actSrc = 100 + a*10 + s;
		if(isDraw())
		{
			diffson->setFirstSource(actSrc);
			sorgenti->draw();
			//diffson->forceDraw();
		}
	}
}

void ambDiffSon::setDraw(bool d)
{
	is_draw = d;
}

bool ambDiffSon::isDraw()
{
	return is_draw;
}
/*****************************************************************
 ** Insieme ambienti diffusione sonora multicanale
 ****************************************************************/

insAmbDiffSon::insAmbDiffSon( QWidget *parent, QPtrList<QString> *names, void *indirizzo,char* Icona1,char* Icona2, QPtrList<dati_ampli_multi> *la, diffSonora *ds, sottoMenu *sorg, diffmulti *dm)
: bannButIcon( parent, (char *)names->at(0)->ascii() )
{
	qDebug("insAmbDiffSon::insAmbDiffSon() : %s %s %s", indirizzo, Icona1, Icona2);
	SetIcons(Icona1, Icona2);
	Draw();
	//setAddress(indirizzo);
	connect(this, SIGNAL(sxClick()), this, SLOT(configura()));
	diffson = ds;
	sorgenti = sorg;
	diffmul = dm;

	QPtrListIterator<dati_ampli_multi> *lai = new QPtrListIterator<dati_ampli_multi>(*la);
	lai->toFirst();
	dati_ampli_multi *am;
	while( ( am = lai->current() ) != 0)
	{
		QPtrList<QString> icons;
		QString qI1(am->I1);
		QString qI2(am->I2);
		QString qI3(am->I3);
		QString qI4(am->I4);
		icons.append(&qI1);
		icons.append(&qI2);
		icons.append(&qI3);
		icons.append(&qI4);

		if(am->tipo == AMPLIFICATORE) {
			qDebug("Adding amplifier (%d, %s %s)", am->tipo, 
					(char *)am->indirizzo, (char *)am->descr->at(0)->ascii());
			diffson->addItem(am->tipo, (char *)am->descr->at(0)->ascii(), 
					(char *)am->indirizzo,
					icons, am->modo);
		} else {
			qDebug("Adding amplifier group(%d)", am->tipo);
			qDebug("indirizzo = %p", am->indirizzo);
			QPtrListIterator<QString> *lii =
				new QPtrListIterator<QString>(*(QPtrList<QString> *)
						(am->indirizzo));
			QString *i;
			lii->toFirst();
			while( ( i = lii->current()) ) {
				QStringList qsl = QStringList::split(QChar(','), *i);
				QPtrList<QString> *indirizzi = new QPtrList<QString>();
				indirizzi->setAutoDelete(true);
				for(int j=0; j<qsl.count(); j++)
					indirizzi->append(new QString(qsl[j]));
				diffson->addItem(am->tipo, (char *)am->descr->at(0)->ascii(), 
						indirizzi,
						icons, am->modo);
				++(*lii);
			}
			delete lii;
		}
		++(*lai);
	}
	delete lai;
}

void insAmbDiffSon::Draw()
{
	qDebug("insAmbDiffSon::Draw()");
	sxButton->setPixmap(*Icon[1]);
	if (pressIcon[1])
		sxButton->setPressedPixmap(*pressIcon[1]);
	BannerIcon->repaint();
	BannerIcon->setPixmap(*(Icon[0]));
	BannerIcon->repaint();
	BannerText->setAlignment(AlignHCenter|AlignVCenter);//AlignTop);
	BannerText->setFont( QFont( "helvetica", 14, QFont::Bold ) );
	BannerText->setText(testo);
}

void insAmbDiffSon::configura()
{
	qDebug("insAmbDiffSon::configura()");
	emit(ambChanged((char *)name(), true, (void *)NULL));
	qDebug("sorgenti->parent() = %p", sorgenti->parent());
	qDebug("disconnecting sorgenti->parent from diffmulti(%p)", diffmul);
	disconnect(sorgenti->parent(), SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	disconnect(sorgenti->parent(), SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	sorgenti->reparent(diffson, 0, diffson->geometry().topLeft());
	connect(diffson, SIGNAL(sendFrame(char *)), diffmul, SIGNAL(sendFrame(char *)));
	connect(diffson, SIGNAL(sendInit(char *)), diffmul, SIGNAL(sendInit(char *)));
	qDebug("connecting diffson(%p) to diffmul(%p)", diffson, diffmul);
	sorgenti->mostra_all(banner::BUT2);
	diffson->forceDraw();
	diffson->showFullScreen();
}


void insAmbDiffSon::actSrcChanged(int a, int s)
{
	qDebug("ambDiffSon::actSrcChanged(%d, %d), ignored", a, s);
}

/*****************************************************************
 ** Sorgente radio diffusione sonora multicanale
 ****************************************************************/
sorgenteMultiRadio::sorgenteMultiRadio( QWidget *parent, const char *name, char* indirizzo, char* Icona1, char* Icona2, char *Icona3, char *ambDescr)
: banradio( parent, name, indirizzo, 3, ambDescr)
{       
	qDebug("sorgenteMultiRadio::sorgenteMultiRadio() : %s %s %s", Icona1, Icona2, Icona3);
	
	SetIcons(Icona1, Icona2, NULL, Icona3);
	
	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	indirizzo_semplice = QString(indirizzo);
	indirizzi_ambienti.clear();
	//connect(this, SIGNAL(dxClick()), this, SLOT(configura()));
	//connect(this, SIGNAL(csxClick()), this, SLOT(cicla()));
	multiamb = false;
}

void sorgenteMultiRadio::attiva()
{
	qDebug("sorgenteMultiRadio::attiva()");
	openwebnet msg_open;
	if(!multiamb) {
		msg_open.CreateMsgOpen("16", "3", getAddress(), "");
		emit sendFrame(msg_open.frame_open);   
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	} else {
		qDebug("DA INSIEME AMBIENTI. CI SONO %d INDIRIZZI",
				indirizzi_ambienti.count());
		for ( QStringList::Iterator it = indirizzi_ambienti.begin(); 
				it != indirizzi_ambienti.end(); ++it ) {
			QString dove = QString(
					QString::number(100 + 
						(*it).toInt() * 10 +
						indirizzo_semplice.toInt(),
						10));
			msg_open.CreateMsgOpen("16", "3", (char *)(dove.ascii()), "");
			emit sendFrame(msg_open.frame_open);   
		}
	}
}

void sorgenteMultiRadio::ambChanged(char *ad, bool multi, void *indamb)
{
	// FIXME: PROPAGA LA VARIAZIONE DI DESCRIZIONE AMBIENTE
	qDebug("sorgenteMultiRadio::ambChanged(%s, %d, %s)", ad, multi, indamb);
	if(!multi) {
		multiamb = false;
		indirizzo_ambiente = QString((const char *)indamb).toInt();
		QString *dove = new QString(
				QString::number(100 + 
					indirizzo_ambiente * 10 +
					indirizzo_semplice.toInt(),
					10));
		qDebug("Source where is now %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
	} else {
		multiamb = true;
		QString *dove = new QString(
				QString::number(100 + 
					indirizzo_semplice.toInt(),
					10));
		qDebug("Source where is now %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
	}
	myRadio->setAmbDescr(ad);
}

void sorgenteMultiRadio::show()
{
	banradio::pre_show();
}

void sorgenteMultiRadio::addAmb(char *a)
{
	qDebug("sorgenteMultiRadio::addAmb(%s)", a);
	indirizzi_ambienti += QString(a);
}

/*****************************************************************
 ** Sorgente aux diffusione sonora multicanale
 ****************************************************************/
	sorgenteMultiAux::sorgenteMultiAux( QWidget *parent,const char *name,char* indirizzo,char* Icona1,char* Icona2, char *Icona3, char *ambdescr)
: sorgente_aux( parent, name, indirizzo, false, ambdescr)
{       
	qDebug("sorgenteMultiAux::sorgenteMultiAux() : "
			"%s %s %s", Icona1, Icona2, Icona3);
	SetIcons(Icona1, Icona2, NULL, Icona3);
	indirizzo_semplice = QString(indirizzo);
	indirizzi_ambienti.clear();
	connect(this, SIGNAL(dxClick()), myAux, SLOT(showAux()));
	connect(this, SIGNAL(sxClick()), this, SLOT(attiva()));
	connect(myAux, SIGNAL(Closed()), myAux, SLOT(hide()));
	connect(myAux, SIGNAL(Closed()), this, SLOT(show()));
	connect(myAux, SIGNAL(Btnfwd()), this, SLOT(aumBrano()));
	multiamb = false;
}

void sorgenteMultiAux::attiva()
{
	qDebug("sorgenteMultiAux::attiva()");
	openwebnet msg_open;
	if(!multiamb) {
		msg_open.CreateMsgOpen("16", "3", getAddress(), "");
		emit sendFrame(msg_open.frame_open);   
		emit active(indirizzo_ambiente, indirizzo_semplice.toInt());
	} else {
		for ( QStringList::Iterator it = indirizzi_ambienti.begin(); 
				it != indirizzi_ambienti.end(); ++it ) {
			QString dove = QString(
					QString::number(100 + 
						(*it).toInt() * 10 +
						indirizzo_semplice.toInt(),
						10));
			msg_open.CreateMsgOpen("16", "3", (char *)(dove.ascii()), "");
			emit sendFrame(msg_open.frame_open);   
		}
	}
}

void sorgenteMultiAux::ambChanged(char *ad, bool multi, void *indamb)
{
	qDebug("sorgenteMultiAux::ambChanged(%s, %d, %s)", ad, multi, indamb);
	if(!multi) {
		multiamb = false;
		indirizzo_ambiente = QString((const char *)indamb).toInt();
		QString *dove = new QString(
				QString::number(100 + 
					QString((const char *)indamb).toInt() * 10 +
					indirizzo_semplice.toInt(),
					10));
		qDebug("Source where now = %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
	} else {
		QString *dove = new QString(
				QString::number(100 + 
					indirizzo_semplice.toInt(),
					10));
		qDebug("Source where is now %s", dove->ascii());
		setAddress((char *)dove->ascii());
		delete dove;
		multiamb = true;
	}
	myAux->setAmbDescr(ad);
}

void sorgenteMultiAux::addAmb(char *a)
{
	qDebug("sorgenteMultiAux::addAmb(%s)", a);
	indirizzi_ambienti += QString(a);
}
