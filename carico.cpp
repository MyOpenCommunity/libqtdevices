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


bannCarico::bannCarico(sottoMenu *parent, QString indirizzo, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannCarico::gestFrame(char*)
{

}

void bannCarico::Attiva()
{
	BTouch->sendFrame(createMsgOpen("3", "2", getAddress()));
}

void bannCarico::inizializza(bool forza)
{
}
