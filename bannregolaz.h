/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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

#endif //BANNREGOLAZ_H
