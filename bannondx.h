/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannondx.h
**
**Riga con tasto ON a dx e scritta da parte
**
****************************************************************/

#ifndef BANNONDX_H
#define BANNONDX_H

#include "banner.h"

class sottoMenu;
class BtButton;
class Page;

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
	virtual ~bannOnDx();
	virtual void inizializza(bool forza=false);

protected:
	virtual void hideEvent(QHideEvent *event);
	Page *linked_page;
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

#endif //BANNONDX_H
