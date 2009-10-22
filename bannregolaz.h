/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bannregolaz.h
**
**Riga con tasto ON OFF, icona centrale divisa in 2 per regolare
**
****************************************************************/

#ifndef BANNREGOLAZ_H
#define BANNREGOLAZ_H

#include "banner.h"

#include <QWidget>

class QTimer;
class BtButton;
class QLabel;


#define BUTREGOL_ICON_DIM_X 120
#define BUTREGOL_ICON_DIM_Y 60
#define BANREGOL_BUT_DIM 60

// substitute for bannRegolaz
class BannAdjust : public BannerNew
{
Q_OBJECT
protected:
	BannAdjust(QWidget *parent);
	void initBanner(const QString &banner_text);
	BtButton *left_button, *center_left_button, *center_right_button, *right_button;

private:
	QLabel *text;
};

/*!
  \class bannRegolaz
  \brief This is a class that describes a banner with a button on the right and on the left and two adjacent buttons on the center (thought to represent each one an half of the same image) and a text in the bottom.
  \author Davide
  \date lug 2005
*/

class bannRegolaz : public banner
{
	Q_OBJECT
public:
	bannRegolaz(QWidget *parent);

private slots:
	void armTimRipdx();
	void armTimRipsx();
	void killTimRipdx();
	void killTimRipsx();

private:
	QTimer *timRip;
};

#endif //BANNREGOLAZ_H
