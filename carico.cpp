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
#include "generic_functions.h" // createMsgOpen

/*****************************************************************
 **carico
 ****************************************************************/

carico::carico(sottoMenu *parent, char *indirizzo, QString IconaSx) : bannOnSx(parent)
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
	BTouch->sendFrame(createMsgOpen("3", "2", getAddress()));
}

void carico::inizializza(bool forza)
{
}
