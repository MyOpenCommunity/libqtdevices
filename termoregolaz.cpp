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

termoregolaz::termoregolaz( QWidget *parent, const char *name, uchar withNavBar ,int width,int  height,uchar n)
        : sottoMenu( parent, name ,withNavBar , width,height, n)
{
}

void termoregolaz::goUp()
{
    if (getNext()->getState()==(unsigned char)S_MAN) 
        setNavBarMode(4,getNext()->getAutoIcon());
    else
        setNavBarMode(4,getNext()->getManIcon());
    sottoMenu::goUp();
}

void termoregolaz::goDown()
{
        if (getPrevious()->getState()==(unsigned char)S_MAN) 
        setNavBarMode(4,getNext()->getAutoIcon());
    else
        setNavBarMode(4,getNext()->getManIcon());
    sottoMenu::goDown();
}
void termoregolaz::show()
{
    if (getCurrent() ->getState()==(unsigned char)S_MAN) 
        setNavBarMode(4,getNext()->getAutoIcon());
    else
        setNavBarMode(4,getNext()->getManIcon());
    draw();
    QWidget::show();    
}
