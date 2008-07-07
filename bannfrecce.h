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

	~bannFrecce();
	/**
	 * Sets the button on the right (BUT2 in banner terminology) to \a btn, allowing each banner to set
	 * the rightmost button on the navbar of its sottoMenu.
	 * This function is called at each sottoMenu::draw() operation on the banner showed on the top (if more than
	 * one banner is displayed).
	 * This function reparents the button \a btn to bannFrecce, sets the correct geometry on the button and hides the previous button
	 * on the right (if present).
	 * If no button is supplied (ie. \a btn = 0), the original button is displayed (ie. the button set with \a num parameter
	 * in the constructor).
	 * \param btn The custom button to be displayed.
	 */
	void setCustomButton(BtButton *btn);

	/**
	 * This function must be reimplemented because the original one always sets the pixmap of the last button. Since
	 * we reparent the custom button with bannFrecce, the icon originally present on the custom button is overwritten.
	 * This function simply draws all the buttons as the parent while the button on the right is drawn only if
	 * it is not a custom button.
	 */
	virtual void Draw();

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
	/// The original button set with the \a num parameter in the constructor
	BtButton *original_dx_button;

	QRect dx_button_coord;

	/// The parent of custom button set with setCustomButton method
	QWidget *dx_button_parent;
};


#define BUTFRECCE_DIM_X 60
#define BUTFRECCE_DIM_Y 60


#endif //BANNFRECCE
