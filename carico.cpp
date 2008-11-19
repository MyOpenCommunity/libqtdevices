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
#include "btmain.h"
#include "main.h" // BTouch

#include <openwebnet.h> // class openwebnet

/*****************************************************************
 **carico
 ****************************************************************/

carico::carico(sottoMenu *parent, const char *name, char *indirizzo, QString IconaSx)
	: bannOnSx(parent, name)
{
	SetIcons(IconaSx, 1);
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
	BTouch->sendFrame(msg_open.frame_open);
}

void carico::inizializza(bool forza)
{
}
