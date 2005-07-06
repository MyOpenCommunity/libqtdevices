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
#include "banner.h"

termoregolaz::termoregolaz( QWidget *parent, const char *name, uchar withNavBar ,int width,int  height,uchar n)
        : sottoMenu( parent, name ,withNavBar , width,height, n)
{
}

void termoregolaz::goUp()
{
        qDebug("goUp");
    if (getNext()->getState()==(unsigned char)S_MAN) 
        setNavBarMode(4,getNext()->getAutoIcon());
    else
        setNavBarMode(4,getNext()->getManIcon());
    sottoMenu::goUp();
 }

void termoregolaz::goDown()
{
    banner * pip;
    unsigned char plof;
    
    qDebug("goDown");
    pip=getPrevious();
    qDebug("goDown 1 ");
//    if (getPrevious()->getState()==(unsigned char)S_MAN) 
    plof=pip->getState();
    qDebug("goDown 2 ");
    if (plof==(unsigned char)S_MAN)         
    {
        qDebug("Termo::goDown 1");
        setNavBarMode(4,getNext()->getAutoIcon());
                qDebug("Termo::goDown 2");
    }
    else
    {
                        qDebug("Termo::goDown 4");
        setNavBarMode(4,getNext()->getManIcon());
                qDebug("Termo::goDown 3");
    }
    sottoMenu::goDown();
}
void termoregolaz::show()
{
    qDebug("TermoShow");
    if (getCurrent() ->getState()==(unsigned char)S_MAN) 
        setNavBarMode(4,getNext()->getAutoIcon());
    else
        setNavBarMode(4,getNext()->getManIcon());
//    forceDraw();
    draw();
    QWidget::show();    
}
