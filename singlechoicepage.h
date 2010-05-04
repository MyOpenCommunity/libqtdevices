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


#ifndef SINGLECHOICEPAGE_H
#define SINGLECHOICEPAGE_H

#include "bannerpage.h"
#include "bann2_buttons.h" // Bann2StateButtons

#include <QButtonGroup>

class QString;
class SingleChoiceContent;
class CheckableBanner;

/*
 * This abstract class is designed to provide a list of bannOnDx. Each of them
 * contains a toggle button that can be toggle in exclusive mode with the other
 * buttons.
 */
class SingleChoicePage : public BannerPage
{
Q_OBJECT
public:
	typedef SingleChoiceContent ContentType;

	// Maybe this should be a QDomNode, but it's used as the base class for BrightnessPage on BTouch
	// which doesn't have a QDomNode in the ctor
	SingleChoicePage(const QString &title = QString());

private slots:
	void okPressed();
	void confirmSelection();

protected:
	// The current id used to choose which button is toggled
	virtual int getCurrentId() = 0;
	// A method called when a button is pressed
	virtual void bannerSelected(int id) = 0;
	// Set the checked button
	void setCheckedId(int id);

protected:
	void addBanner(CheckableBanner *bann, int id);
};



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

#endif // SINGLECHOICEPAGE_H
