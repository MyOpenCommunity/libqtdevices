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
#include "device.h"
#include "banner.h"
#include "banntermo.h"

termoregolaz::termoregolaz( QWidget *parent, const char *name, uchar withNavBar ,int width,int  height,uchar n)
: sottoMenu( parent, name ,withNavBar , width,height, n)
{
}


void termoregolaz::goUp()
{
	bannTermo *bn = (bannTermo *)getNext();

	if (bn->devtype == bannTermo::THERMO_99_ZONES || bn->devtype == bannTermo::THERMO_99_ZONES_FANCOIL)
	{
		switch (bn->getState())
		{
		case device_status_thermr::S_MAN:
			setNavBarMode(4, bn->getAutoIcon());
			break;
		case device_status_thermr::S_AUTO:
			setNavBarMode(4, bn->getManIcon());
			break;
		case device_status_thermr::S_TERM:
		default:
			setNavBarMode(3, (char *)"");
			break;
		}
	}
	else
		setNavBarMode(3, (char *)"");

	sottoMenu::goUp();
}


void termoregolaz::goDown()
{
	bannTermo *bn = (bannTermo *)getPrevious();

	if (bn->devtype == bannTermo::THERMO_99_ZONES || bn->devtype == bannTermo::THERMO_99_ZONES_FANCOIL)
	{
		switch (bn->getState())
		{
		case device_status_thermr::S_MAN:
			setNavBarMode(4, bn->getAutoIcon());
			break;
		case device_status_thermr::S_AUTO:
			setNavBarMode(4, bn->getManIcon());
			break;
		case device_status_thermr::S_TERM:
		default:
			setNavBarMode(3, (char *)"");
			break;
		}
	}
	else
		setNavBarMode(3, (char *)"");

	sottoMenu::goDown();
}


void termoregolaz::show()
{
	qDebug("TermoShow");

	bannTermo *bn = (bannTermo *)getCurrent();

	if (bn->devtype == bannTermo::THERMO_99_ZONES || bn->devtype == bannTermo::THERMO_99_ZONES_FANCOIL)
	{
		switch (bn->getState())
		{
		case device_status_thermr::S_MAN:
			setNavBarMode(4, bn->getAutoIcon());
			break;
		case device_status_thermr::S_AUTO:
			setNavBarMode(4, bn->getManIcon());
			break;
		case device_status_thermr::S_TERM:
		default:
			setNavBarMode(3, (char *)"");
			break;
		}
	}
	else
		setNavBarMode(3, (char *)"");

	forceDraw();
	QWidget::show();
}

