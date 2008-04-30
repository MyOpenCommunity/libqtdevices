/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** termoregolaz.cpp
 **
 **Rsottomenu termoregolazione
 **
 ****************************************************************/

#include <qtimer.h>

#include "termoregolaz.h"
#include "main.h"
#include "items.h"
#include "device.h"
#include "banner.h"

termoregolaz::termoregolaz( QWidget *parent, const char *name, uchar withNavBar ,int width,int  height,uchar n)
: sottoMenu( parent, name ,withNavBar , width,height, n)
{
}


void termoregolaz::goUp()
{
	qDebug("goUp");

	switch(getNext() ->getState()) 
	{
	case device_status_thermr::S_MAN:
		setNavBarMode(4,getNext()->getAutoIcon());
		break;
	case device_status_thermr::S_AUTO:
		setNavBarMode(4,getNext()->getManIcon());
		break;
	case device_status_thermr::S_TERM:
	default:
		setNavBarMode(3, (char *)"");
		break;
	}

	sottoMenu::goUp();
}


void termoregolaz::goDown()
{
	qDebug("goDown");

	switch(getPrevious() ->getState()) 
	{
	case device_status_thermr::S_MAN:
		setNavBarMode(4,getPrevious()->getAutoIcon());
		break;
	case device_status_thermr::S_AUTO:
		setNavBarMode(4,getPrevious()->getManIcon());
		break;
	case device_status_thermr::S_TERM:
	default:
		setNavBarMode(3, (char *)"");
		break;
	}

	sottoMenu::goDown();
}


void termoregolaz::show()
{
	qDebug("TermoShow");

	switch(getCurrent() ->getState()) 
	{
	case device_status_thermr::S_MAN:
		setNavBarMode(4,getCurrent()->getAutoIcon());
		break;
	case device_status_thermr::S_AUTO:
		setNavBarMode(4,getCurrent()->getManIcon());
		break;
	case device_status_thermr::S_TERM:
	default:
		setNavBarMode(3, (char *)"");
		break;
	}

	forceDraw();
	QWidget::show();
}

