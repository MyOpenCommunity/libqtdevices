/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** automatismi.cpp
 **
 **definizioni dei vari automatismi implementati
 **
 ****************************************************************/

#include "automatismi.h"
#include "../bt_stackopen/common_files/openwebnet.h" // class openwebnet
#include "device_cache.h" // btouch_device_cache


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
	dev->sendFrame(msg_open.frame_open);
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
	dev->sendFrame(msg_open.frame_open);
}
