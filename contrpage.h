/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** contrPage.h
**
**definizioni della pagina di definizione contrasto
**
****************************************************************/

#ifndef CONTRPAGE_H
#define CONTRPAGE_H

#include "page.h"

class BtButton;
class QLabel;

/*!
  \class contrPage
  \brief This is a class that manage the contrast of the LCD. 

  It occupies an etire page, shows a color bar in the middle and has three buttons on the bottom to increase/decrease the contrast and to confirm the value chosen.
  \author Davide
  \date lug 2005
*/

class contrPage : public Page
{
Q_OBJECT
public:
	contrPage();
	void draw();

	BtButton *aumBut, *decBut, *okBut;
	QLabel *paintLabel, *colorBar;

private slots:
	void aumContr();
	void decContr();
};

#define BUT_DIM 60
#define IMG_X 181
#define IMG_Y 128

#endif // CONTRPAGE_H
