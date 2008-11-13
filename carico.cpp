/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** carico.cpp
 **
 **
 **
 ****************************************************************/

#include "carico.h"
#include "device_cache.h"
#include "device.h"

#include <openwebnet.h> // class openwebnet

/*****************************************************************
 **carico
 ****************************************************************/

carico::carico(sottoMenu *parent, const char *name, char *indirizzo, QString IconaSx)
	: bannOnSx(parent, name)
{

	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	dev = btouch_device_cache.get_device(getAddress());
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
	dev->sendFrame(msg_open.frame_open);
}

void carico::inizializza(bool forza){}
