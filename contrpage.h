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

#include "btlabel.h"
#include "btbutton.h"
#include "main.h"

#include <qframe.h>
#include <qcursor.h>

/*!
  \class contrPage
  \brief This is a class that manage the contrast of the LCD. 

  It occupies an etire page, shows a color bar in the middle and has three buttons on the bottom to increase/decrease the contrast and to confirm the value chosen.
  \author Davide
  \date lug 2005
*/

class  contrPage : public QWidget
{
   Q_OBJECT
public:
    contrPage(QWidget *parent=0, const char *name=0);

/*!
  \brief Sets the background color for the banner.

  The arguments are RGB components for the color.
*/
	virtual void setBGColor(int, int , int);

/*!
  \brief Sets the foreground color for the banner.

  The arguments are RGB components for the color.
*/

	virtual void setFGColor(int , int , int);

/*!
  \brief Sets the background color for the banner.

  The argument is the QColor description of the color.
*/
	virtual void setBGColor(QColor);

/*!
  \brief Sets the foreground color for the banner.

  The argument is the QColor description of the color.
*/
	virtual void setFGColor(QColor);

/*!
  \brief Sets the background pixmap for the banner.
*/
	int setBGPixmap(char*);

	BtButton *aumBut, *decBut,*okBut;
	BtLabel* paintLabel,*colorBar;
	void draw();

signals:
	void Close();

private slots:
	void aumContr();
	void decContr();

};

#define BUT_DIM 60
#define IMG_X 181
#define IMG_Y 128

#endif // CONTRPAGE_H
