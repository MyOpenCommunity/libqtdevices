/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** termoregolaz.cpp
**
**Rsottomenu termoregolazione
**
****************************************************************/

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
#if 1 
    switch(getNext() ->getState()) {
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
#else
    if (getNext()->getState()==device_status_thermr::S_MAN) 
        setNavBarMode(4,getNext()->getAutoIcon());
    else
        setNavBarMode(4,getNext()->getManIcon());
#endif
    sottoMenu::goUp();
 }

void termoregolaz::goDown()
{
#if 0
    banner * pip;
    unsigned char plof;
#endif
    
    qDebug("goDown");
#if 1 
    switch(getPrevious() ->getState()) {
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
#else
    pip=getPrevious();
    qDebug("goDown 1 ");
//    if (getPrevious()->getState()==(unsigned char)S_MAN) 
    plof=pip->getState();
    qDebug("goDown 2 ");
    if (plof==device_status_thermr::S_MAN)         
    {
        qDebug("Termo::goDown 1");
        setNavBarMode(4,getPrevious()->getAutoIcon());
                qDebug("Termo::goDown 2");
    }
    else
    {
                        qDebug("Termo::goDown 4");
        setNavBarMode(4,getPrevious()->getManIcon());
                qDebug("Termo::goDown 3");
    }
#endif
    sottoMenu::goDown();
}

void termoregolaz::show()
{
    qDebug("TermoShow");
#if 1 
    switch(getCurrent() ->getState()) {
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
#else
    if (getCurrent() ->getState()==device_status_thermr::S_MAN) 
        setNavBarMode(4,getCurrent()->getAutoIcon());
    else
        setNavBarMode(4,getCurrent()->getManIcon());
#endif
    forceDraw();
    //draw();
    QWidget::show();    
}
