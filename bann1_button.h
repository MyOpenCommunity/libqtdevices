/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann1_button.h
**
** In this file there are contained all generic banners with 1 button.
**
****************************************************************/

#ifndef BANN1_BUTTON_H
#define BANN1_BUTTON_H

#include "banner.h"

class sottoMenu;
class BtButton;
class Page;
class QWidget;
class TextOnImageLabel;


#define BANPULS_BUT_DIM  60
#define BANPULS_ICON_DIM_X 120
#define BANPULS_ICON_DIM_Y 60


/*!
  \class bannPuls
  \brief This is a class that describes a banner with a button on the right, an icon in the center and a text on the bottom
  \author Davide
  \date lug 2005
*/

class bannPuls : public banner
{
Q_OBJECT
public:
	bannPuls(QWidget *parent);

signals:
	void click();
	void pressed();
	void released();
};

/*!
  \class bannSimple
  \brief A very simple banner with only a button in the center.
*/

class bannSimple : public banner
{
Q_OBJECT
public:
	bannSimple(sottoMenu *parent, QString icon=QString(), Page *page=0);

signals:
	void click();
};


/*!
  \class bannOnDx
  \brief This is a class that describes a banner with a button on the right and a text on the remaining area
  \author Davide
  \date lug 2005
*/
class bannOnDx : public banner
{
Q_OBJECT
public:
	bannOnDx(sottoMenu *parent, QString icon=QString(), Page *page=0);

signals:
	void click();
};


/*!
  \class bannOnSx
  \brief This is a class that describes a banner with a button on the left and a text on the remaining area
  \author Davide
  \date lug 2005
*/
class bannOnSx : public banner
{
Q_OBJECT
public:
	bannOnSx(sottoMenu *parent, QString icon=QString());
	BtButton *getButton();
signals:
	void click();
};



class bannOnIcons : public banner
{
Q_OBJECT
public:
	bannOnIcons(QWidget *parent);
};


/*!
  \class bannOn2scr
  \brief This is a class that describes a banner with a button on the right, text on the bottom area and on the left, plus a couple of icons in the middle
  \author Ciminaghi
  \date apr 2006
*/
class bannOn2scr : public banner
{
Q_OBJECT
public:
	bannOn2scr(QWidget *w=0);
};


/*!
  \class bannBut2Icon
  \brief This is a class that describes a banner with a button on the right,
  two icons in the middle and some text below
  \author Ciminaghi
  \date jun 2006
*/
class bannBut2Icon : public banner
{
Q_OBJECT
public:
	bannBut2Icon(QWidget *);
};


class bannTextOnImage : public banner
{
Q_OBJECT
public:
	bannTextOnImage(QWidget *parent, const QString &text = QString());
	void setInternalText(const QString &text);

private:
	TextOnImageLabel *label;
};

#endif // BANN1_BUTTON_H
