/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief ???
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef SMFULLSCREEN_H
#define SMFULLSCREEN_H

#include "banner.h"
#include <qlabel.h>

#define BUTMENPLUS_DIM_X	 	60
#define BUTMENPLUS_DIM_Y	 	60
#define BUTAUTOMAN_DIM_X	 	60
#define BUTAUTOMAN_DIM_Y	 	60
#define DESCR_DIM_Y		 	20
#define TEMPMIS_Y		 	80
#define OFFSET_Y		 	40


class BannFullScreen : public banner
{
Q_OBJECT
public:
	BannFullScreen(QWidget *parent, const char *name, QColor bg, QColor fg, QColor second_fg);
	virtual void Draw();
	// void callmeBack() call this from sottoMenu to setNavBarMode with the correct icon
	virtual void postDisplay();

private:
	/// Measured temperature label and string
	QLabel *temp_label;
	QString temp;
	/// Zone description label and string
	QLabel *descr_label;
	QString descr;

};

#endif // SMFULLSCREEN_H
