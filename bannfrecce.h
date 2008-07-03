/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannonoff.h
**
**Riga con tasto ON OFF, icona centrale e scritta sotto
**
****************************************************************/

#ifndef BANNFRECCE
#define BANNFRECCE

#include <qtimer.h>

#include "banner.h"
#include "main.h"

/*!
  \class bannFrecce
  \brief This is a class that describes the navigation bar which can have up to 4 buttons.

  There can be 9 different kind of banner combinig the necessities of the various pages.
  \author Davide
  \date lug 2005
*/

class bannFrecce : public banner
{
    Q_OBJECT
public:
/*!
  the different kinds of navigation bar that can be made are depend on the third argument of the constructor (num):
  num=1: one button on the left with the left-arrow image;
  num=2: like "1" with in addition one button on the center-left with the up-arrow image;
  num=3: like "2" with in addition one button on the center-right with the down-arrow image;
  num=4: like "3" with in addition one button on the right with the IconBut4 image;
  num=5: like "4" with the ok-button on the left instead of the left-arrow one;
  num=6: like "3" with the ok-button on the left instead of the left-arrow one;
  num=7: like "2" with the ok-button on the left instead of the left-arrow one;
  num=8: like "1" with the ok-button on the left instead of the left-arrow one;
  num=9: one button on the left with the ok-button image and one button on the right with the IconBut4 image;
  num=10: like "9" with arrow left image on the left instead of ok button;

  if IconBut4 isn't specified the right arrow is shown by default
*/
	bannFrecce( QWidget * parent=NULL, const char *name=NULL ,uchar num=3,char* IconBut4=ICON_FRECCIA_DX);
	void setCustomButton(BtButton *btn);

signals:
	void backClick();
	void upClick();
	void downClick();
	void forwardClick();

	void backPress();
	void upPress();
	void downPress();
	void forwardPress();

	void backRelease();
	void upRelease();
	void downRelease();
	void forwardRelease();

	void longBackPress();

private slots:
	void handleBackPress();
	void handleBackRelease();

private:
	QTimer press_timer;
	BtButton *originalDxButton;
};


#define BUTFRECCE_DIM_X 60
#define BUTFRECCE_DIM_Y 60


#endif //BANNFRECCE
