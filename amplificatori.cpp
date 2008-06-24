/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** amplificatori.cpp
 **
 **definizioni dei vari amplificatori implementati
 **
 ****************************************************************/

#include "amplificatori.h"
#include "../bt_stackopen/common_files/openwebnet.h" // class openwebnet
#include "device_cache.h" // btouch_device_cache
#include "sottomenu.h"

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


void amplificatore:: Accendi()
{
	char    pippo[50];
	char ind[3];

	qDebug("amplificatore::Accendi()");
	memset(pippo,'\000',sizeof(pippo));
	sprintf(ind, "%s", getAddress());
	sprintf(pippo,"*22*34#4#%c*3#%c#%c##",ind[0], ind[0], ind[1]);
	dev->sendFrame(pippo);
}
void amplificatore:: Spegni()
{
	char    pippo[50];
	char ind[3];

	qDebug("amplificatore::Spegni()");
	memset(pippo,'\000',sizeof(pippo));
	sprintf(ind, "%s", getAddress());
	sprintf(pippo,"*22*0#4#%c*3#%c#%c##",ind[0], ind[0], ind[1]);
	dev->sendFrame(pippo);
}
void amplificatore:: Aumenta()
{
	openwebnet msg_open;

	qDebug("amplificatore::Aumenta()");
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("16", "1001",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}
void amplificatore:: Diminuisci()	
{
	openwebnet msg_open;

	qDebug("amplificatore::Diminuisci()");
	msg_open.CreateNullMsgOpen();
	msg_open.CreateMsgOpen("16", "1101",getAddress(),"");
	dev->sendFrame(msg_open.frame_open);
}
void amplificatore::inizializza(bool forza)
{ 
	openwebnet msg_open;
	char    pippo[50];

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*1##");
	//   qDebug("mando frame ampli %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	dev->sendInit(msg_open.frame_open);

	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#16*");
	strcat(pippo,getAddress());
	strcat(pippo,"*5##");
	//   qDebug("mando frame ampli %s",pippo);
	msg_open.CreateMsgOpen((char*)&pippo[0],strlen((char*)&pippo[0]));
	dev->sendInit(msg_open.frame_open);

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
	dev = btouch_device_cache.get_device(getAddress());
	connect(this,SIGNAL(sxClick()),this,SLOT(Attiva()));
	connect(this,SIGNAL(dxClick()),this,SLOT(Disattiva()));
	connect(this,SIGNAL(cdxClick()),this,SLOT(Aumenta()));
	connect(this,SIGNAL(csxClick()),this,SLOT(Diminuisci()));
}



void grAmplificatori::setAddress(void*indirizzi)
{
	elencoDisp=*((QPtrList<QString>*)indirizzi);
}


void grAmplificatori::Attiva()
{
	char pippo[50];
	char ind[3];

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		memset(pippo,'\000',sizeof(pippo));
		sprintf(ind, "%s", (char*)elencoDisp.at(idx)->ascii());
		if(strcmp(ind, "0") == 0)
			sprintf(pippo,"*22*34#4#%c*5#3#%c##",ind[0], ind[0]);
		else if(ind[0] == '#')
			sprintf(pippo,"*22*34#4#%c*4#%c##",ind[1], ind[1]);
		else
			sprintf(pippo,"*22*34#4#%c*3#%c#%c##",ind[0], ind[0], ind[1]);
		dev->sendFrame(pippo);
	}
}

void grAmplificatori::Disattiva()
{
	char pippo[50];
	char ind[3];

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		memset(pippo,'\000',sizeof(pippo));
		sprintf(ind, "%s", (char*)elencoDisp.at(idx)->ascii());
		if(strcmp(ind, "0") == 0)
			sprintf(pippo,"*22*0#4#%c*5#3#%c##",ind[0], ind[0]);
		else if(ind[0] == '#')
			sprintf(pippo,"*22*0#4#%c*4#%c##",ind[1], ind[1]);
		else
			sprintf(pippo,"*22*0#4#%c*3#%c#%c##",ind[0], ind[0], ind[1]);
		dev->sendFrame(pippo);
	}
}

void grAmplificatori::Aumenta()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("16", "1001",(char*)elencoDisp.at(idx)->ascii(),"");
		dev->sendFrame(msg_open.frame_open);
	}
}

void grAmplificatori::Diminuisci()
{
	openwebnet msg_open;

	for(uchar idx=0; idx<elencoDisp.count();idx++)
	{
		msg_open.CreateNullMsgOpen();
		msg_open.CreateMsgOpen("16", "1101",(char*)elencoDisp.at(idx)->ascii(),"");
		dev->sendFrame(msg_open.frame_open);
	}
}
void grAmplificatori::inizializza(bool forza)
{ 
}
