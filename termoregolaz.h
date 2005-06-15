/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** termoregolaz.h
**
** sottomenu termoregolazione
**
****************************************************************/

#ifndef  TERMOREGOLAZZZZZIONE_H
#define TERMOREGOLAZZZZZIONE_H

#include "main.h"
#include "items.h"
#include "sottomenu.h"

class termoregolaz : public sottoMenu
{
    Q_OBJECT
public:
   termoregolaz( QWidget *parent=0, const char *name=0, uchar withNavBar=3 ,int width=MAX_WIDTH,int  height=MAX_HEIGHT,uchar n=NUM_RIGHE-1);
public slots:
     void goUp();
     void goDown();
     void show();
signals:
private:
};

#endif //TERMOREGOLAZ



