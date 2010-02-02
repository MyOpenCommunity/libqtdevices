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

#include <QTimer>

class BtButton;
class QLabel;


#define BUTREGOL_ICON_DIM_X 120
#define BUTREGOL_ICON_DIM_Y 60
#define BANREGOL_BUT_DIM 60

// substitute for bannRegolaz
/*
 * A class to control the rate of signals emitted
 */
class BannLevel : public BannerNew
{
Q_OBJECT
protected:
	BannLevel(QWidget *parent);
	// to be used by derived classes which are still graphics (eg. AdjustDimmer)
	void initBanner(const QString &banner_text);
	// use this when no state is required
	void initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &banner_text);
	void setCenterLeftIcon(const QString &image);
	void setCenterRightIcon(const QString &image);
	BtButton *left_button, *right_button;

private slots:
	void startLeftTimer();
	void startRightTimer();

private:
	BtButton *center_left_button, *center_right_button;
	QLabel *text;
	QTimer timer;

signals:
	void center_left_clicked();
	void center_right_clicked();
};

/*!
  \class bannRegolaz
  \brief This is a class that describes a banner with a button on the right and on the left and two adjacent buttons on the center (thought to represent each one an half of the same image) and a text in the bottom.
  \author Davide
  \date lug 2005
*/

class bannRegolaz : public BannerOld
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
