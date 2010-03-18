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


#ifndef SINGLECHOICECONTENT_H
#define SINGLECHOICECONTENT_H

#include "bannercontent.h"
#include "bann2_buttons.h" // Bann2StateButtons

#include <QButtonGroup>


class CheckableBanner : public Bann2StateButtons
{
public:
	CheckableBanner(const QString &text, const QString &right_icon = QString());

	BtButton *getButton();
};


namespace SingleChoice
{
	CheckableBanner *createBanner(const QString &text, const QString &right_icon=QString());
}


class SingleChoiceContent : public BannerContent
{
Q_OBJECT
public:
	SingleChoiceContent();

	// Get the active button
	int checkedId() const;
	// add a new banner to the widget
	void addBanner(CheckableBanner *bann, int id);

	// returns the list of buttons for customization
	QList<QAbstractButton*> getButtons();

public slots:
	// Set the active button
	void setCheckedId(int id);

signals:
	void bannerSelected(int id);

private:
	QButtonGroup buttons;
};

#endif // SINGLECHOICECONTENT_H
