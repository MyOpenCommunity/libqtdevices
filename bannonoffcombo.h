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


#ifndef BANNONOFF_COMBO
#define BANNONOFF_COMBO

#include "banner.h"
#include <QWidget>

class BtButton;
class QLabel;

enum ComboStatus
{
	CENTER = 0,
	SX,
	DX,
	NUM_STATUS // fake status, used to get enum length
};


class BannOnOffCombo : public Banner
{
Q_OBJECT
protected:
	BannOnOffCombo(QWidget *w=0);
	void initBanner(const QString &left, const QString center_left, const QString &center,
		const QString &center_right, const QString &right, ComboStatus init_status, const QString &banner_text);
	void changeStatus(ComboStatus st);
	void setInternalText(const QString &t);
	BtButton *left_button, *right_button;

private:
	ScrollingLabel *text, *internal_label;
	QLabel *left_icon, *center_icon, *right_icon;
	QString status_icon[NUM_STATUS];
};

#endif
