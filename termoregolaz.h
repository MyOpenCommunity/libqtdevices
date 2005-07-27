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
/*!
  \class termoregolaz
  \brief This class is the implementation of the Thermoregulation subtree.

  It is essentially a \a sottoMenu that reimplements show(), goUp() and goDown() methods. This is necessary to align the auto/man button which is placed in the navigation bar. This isn't very good because navigation bar is a subtree information while auto-man state is tipical of the banner, but this solution matches with the software specifications.
  \author Davide
  \date lug 2005
*/  
class termoregolaz : public sottoMenu
{
    Q_OBJECT
public:
   termoregolaz( QWidget *parent=0, const char *name=0, uchar withNavBar=3 ,int width=MAX_WIDTH,int  height=MAX_HEIGHT,uchar n=NUM_RIGHE-1);
public slots:
 /*!
  \brief Reimplements sottoMenu::goUp() aligning navigation bar appearence knowing the auto-man state of the zone actually shown.
*/      
     void goUp();
 /*!
  \brief Reimplements sottoMenu::goUp() aligning navigation bar appearence knowing the auto-man state of the zone actually shown.
*/  
     void goDown();
 /*!
  \brief Reimplements sottoMenu::goUp() aligning navigation bar appearence knowing the auto-man state of the first thermoregulation zone.
*/       
     void show();
signals:
private:
};

#endif //TERMOREGOLAZ



