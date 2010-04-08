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


#ifndef BANN2_BUTTONS_H
#define BANN2_BUTTONS_H

#include "banner.h"
#include "fontmanager.h" // FontManager

class QWidget;
class QLabel;
class BtButton;
class TextOnImageLabel;


/*
 * Small base class for all 2 buttons banners that can have linked pages.
 */
class Bann2LinkedPages : public BannerNew
{
Q_OBJECT
public:
	Bann2LinkedPages(QWidget *parent = 0);
	void connectLeftButton(Page *p);
	void connectRightButton(Page *p);

protected:
	BtButton *left_button, *right_button;
};


/*
 * This class is a new style banner which creates and has handles to its own buttons
 * and labels.
 * New style banners have business logic and graphics divided in two classes: the graphics class
 * (base class for logic banners) will expose to logic banners a simple interface:
 * - one function to set icons: the graphics banner knows how to load icons;
 * - one to set text on each label that has text;
 * - one function to change state: icon changes are managed by logic banners only through
 * this function.
 * These are only guidelines, each graphics banner can declare its own interface (for example
 * to set the volume level or dimmer level). However you are invited to make the interface
 * similar to this one to maintain a standard coding style.
 * Buttons are created protected so that logic banners can manipulate them directly, thus
 * avoiding BtButton interface duplication. All other elements are created private.
 */

/**
 * Two buttons on the sides + description in the center.
 * Either button can be removed by giving an empty string as the icon parameter to initBanner()
 * Nicer replacement for bann2but, will replace also bannOnDx, bannOnSx
 * Now it can replace BannSinglePuls also.
 */
class Bann2Buttons : public Bann2LinkedPages
{
Q_OBJECT
public:
	Bann2Buttons(QWidget *parent = 0);

	/**
	 * \param left icon path for the left button
	 * \param right icon path for the right button
	 * \param banner_text text of the label between the two buttons
	 * \param banner_description text of the label below the banner; if empty, the label is deleted
	 */
	void initBanner(const QString &left, const QString &right, const QString &banner_text,
		FontManager::Type text_font = FontManager::TEXT, const QString &banner_description = QString(),
		FontManager::Type description_font = FontManager::TEXT);

	/**
	 * \param left icon path for the left button
	 * \param center icon path for the banner background
	 * \param right icon path for the right button
	 * \param description text of the label below the banner; if empty, the label is deleted
	 */
	void initBanner(const QString &left, const QString &center, const QString &right, const QString &description,
			FontManager::Type description_font = FontManager::TEXT);

	// can only be caller after initBanner()
	void setCentralText(const QString &t);
	void setDescriptionText(const QString &t);

signals:
	void rightClicked();
	void leftClicked();

protected:
	void setBackgroundImage(const QString &i);

private:
	// only one of center_icon/center_label is created, depending on which initBanner() is called
	TextOnImageLabel *center_icon;
	QLabel *center_label;
	QLabel *description;
};


/**
 * A class similar to the BannOnOffNew class that changes the state and thus the center icon.
 */
class BannOnOffState : public Bann2Buttons
{
Q_OBJECT
public:
	enum States
	{
		ON,
		OFF,
	};

	BannOnOffState(QWidget *parent);
	void initBanner(const QString &left, const QString &center, const QString &right,
		States init_state, const QString &banner_text);
	void setState(States new_state);

private:
	QString center;
};


class BannOnOff2Labels : public BannerNew
{
Q_OBJECT
protected:
	enum States
	{
		ON,
		OFF,
	};
	BannOnOff2Labels(QWidget *parent);
	void initBanner(const QString &left, const QString &_center, const QString &right,
		States init_state, const QString &banner_text, const QString &second_text);
	void setCentralText(const QString &str);
	void setCentralTextSecondaryColor(bool secondary);
	void setState(States new_state);

	BtButton *right_button, *left_button;
private:
	QLabel *center_icon, *center_text, *text;
	QString center;
};

class BannOpenClose : public BannerNew
{
Q_OBJECT
protected:
	enum States
	{
		STOP,
		CLOSING,
		OPENING,
	};

	BannOpenClose(QWidget *parent);
	/*
	 * The 'real' banner constructor. Sets icons, initial state and banner text.
	 * Since text is fixed, no interface function to manipulate it is given.
	 */
	void initBanner(QString left, QString center, QString right, QString lr_alternate,
		States starting_state, QString banner_text);
	void setState(States new_state);

	BtButton *left_button, *right_button;

private:
	void loadIcons(QString _left, QString _center, QString _right, QString _lr_alternate);

	QString left, center, right;
	// alternative icon for left *and* right buttons. If buttons need different
	// icons, we need to split it.
	QString alternate;
	QLabel *text, *center_icon;
};

class Bann2CentralButtons : public BannerNew
{
Q_OBJECT
protected:
	Bann2CentralButtons(QWidget *parent);
	void initBanner(const QString &left, const QString &right);
	BtButton *center_left, *center_right;
};


/*!
  \class bann2But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \date lug 2005
*/
class bann2But : public banner
{
Q_OBJECT
public:
	bann2But(QWidget *parent);
};


/*!
  \class bannOnOff
  \brief This is a class that describes a banner with a button on the right and on the left, an icon on the center and a text on the bottom
  \date lug 2005
*/
class bannOnOff : public banner
{
Q_OBJECT
public:
	bannOnOff(QWidget *w=0);
};


/*!
  \class bannOnOff2scr
  \brief This is a class that describes a banner with a button on the right and on the left and a text on the bottom area and an other the center-left
  \date lug 2005
*/
class bannOnOff2scr : public banner
{
Q_OBJECT
public:
	bannOnOff2scr(QWidget *w=0);
};

#endif // BANN2_BUTTONS_H
