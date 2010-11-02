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
#include "homewindow.h" // TrayBar

class QWidget;
class QLabel;
class QLCDNumber;
class BtButton;
class TextOnImageLabel;
class StateButton;
class ScrollingLabel;


// Small base class for all 2 buttons banners that can have linked pages.
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
 *
 * The function that initializes the graphical banner is conventionally called initBanner(); it
 * sets icons and text on the banner. We need a separate function from constructor since some information
 * needed to initialize the graphic banner may not be ready when the banner is constructed.
 * TODO: maybe we could use initialization lists?
 *
 * These are only guidelines, each graphics banner can declare its own interface (for example
 * to set the volume level or dimmer level). However you are invited to make the interface
 * similar to this one to maintain a standard coding style.
 * Buttons are created protected so that logic banners can manipulate them directly, thus
 * avoiding BtButton interface duplication. All other elements are created private.
 */

/*!
	\ingroup Core
	\brief A \ref banner with two buttons on the sides and a description in the center.

	The %Bann2Buttons is the base class to choose for those banners that have
	a description in the center and a button on the left or a button on the right
	(or both). Optionally, it can draw a description below the banner.
*/
class Bann2Buttons : public Bann2LinkedPages
{
Q_OBJECT
public:
	Bann2Buttons(QWidget *parent = 0);

	/*!
		\brief Initialize the banner.

		Uses \a left and \a right as icon path for the left/right button, \a banner_text
		as the text of the label between the two buttons and a \a banner_description
		as the text of the label below the banner.
	*/
	void initBanner(const QString &left, const QString &right, const QString &banner_text,
		FontManager::Type text_font = FontManager::TEXT, const QString &banner_description = QString(),
		FontManager::Type description_font = FontManager::BANNERDESCRIPTION);

	/*!
		\brief Initialize the banner.

		Uses \a left and \a right as icon path for the left/right button, \a center
		as the path for the banner background and \a description as the text
		of the label below the banner.
	*/
	void initBanner(const QString &left, const QString &center, const QString &right, const QString &description,
			FontManager::Type description_font = FontManager::BANNERDESCRIPTION);

	// can only be called after initBanner()
	void setTextAlignment(Qt::Alignment align);
	void setCentralText(const QString &t);
	void setDescriptionText(const QString &t);

signals:
	void rightClicked();
	void leftClicked();

protected:
	void setBackgroundImage(const QString &i);

	// an overloaded constructor to change the class of left/right buttons
	template<class ButtonType>
	Bann2Buttons(QWidget *parent, ButtonType *dummy) :
		Bann2LinkedPages(parent)
	{
		left_button = new ButtonType;
		right_button = new ButtonType;

		createBanner();
	}

	// only one of center_icon/center_label is created, depending on which initBanner() is called
	TextOnImageLabel *center_icon;
	ScrollingLabel *center_label;
	ScrollingLabel *description;

private:
	void createBanner();
};


/*!
	\ingroup Core
	\brief A Bann2Buttons where the two buttons are StateButton%s.
*/
class Bann2StateButtons : public Bann2Buttons
{
public:
	Bann2StateButtons(QWidget *parent = 0);

protected:
	StateButton *left_button, *right_button;
};


// A class similar to the BannOnOffNew class that changes the state and thus the center icon.
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

protected:
	// an overloaded constructor to change the class of left/right buttons
	template<class ButtonType>
	BannOnOffState(QWidget *parent, ButtonType *dummy) :
		Bann2Buttons(parent, dummy)
	{
	}

private:
	QString center;
};


// Banner with 2 buttons (left + right), a text on center-left, an icon on center-right and a text below.
// Has two states: ON and OFF
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

	/*
	 * \param left Left button icon path
	 * \param _center Center icon path. Given path will be shown during ON state.
	 * \param right Right button icon path
	 * \param init_state Banner initial state
	 * \param banner_text Text below the banner
	 * \param second_text Initial center text
	 */
	void initBanner(const QString &left, const QString &_center_on, const QString &_center_off, const QString &right,
		States init_state, const QString &banner_text, const QString &second_text);
	/*
	 * Set the text between left button and center right icon
	 */
	void setCentralText(const QString &str);

	/*
	 * Choose if secondary text should be shown in secondary color
	 *
	 * Set "SecondFgColor" property to bool value given as parameter. Actual text color will be set using
	 * stylesheets.
	 * \param secondary True if text should be shown in secondary color, false otherwise.
	 */
	void setCentralTextSecondaryColor(bool secondary);

	/*
	 * Change banner state; this will change center right icon only.
	 *
	 * Icon for the OFF state will be deduced from the ON state icon, "off" will be appended to such path.
	 * Eg. if ON state icon path is "icon.png", for off state an icon called "iconoff.png" will be searched.
	 */
	void setState(States new_state);

	BtButton *right_button, *left_button;
private:
	QLabel *center_icon, *center_text, *text;
	QString center_on, center_off;
};

/*
 * Banner with 2 buttons on both sides, an icon on the center, a description below and three states.
 *
 * Banner behaviour on state changes:
 * STOP: 'normal' icon on buttons, 'base' icon on the center
 * CLOSING: 'normal' icon on right button, 'alternate' icon on left button, 'closing' icon on center
 * OPENING: 'normal' icon on left, 'alternate' icon on right, 'opening' icon on center
 *
 * See functions below for a description of 'normal', 'base', 'alternate' etc. terms.
 */
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
	/*
	 * \param left 'Normal' icon for left button
	 * \param center 'Base' icon for center icon
	 * \param right 'Normal' icon for right button
	 * \param lr_alternate 'Alternate' icon for both left and right buttons
	 * \param starting_state Initial state of the banner
	 * \param banner_text Banner description below
	 */
	void initBanner(QString left, QString center, QString right, QString lr_alternate,
		States starting_state, QString banner_text);
	/*
	 * Change banner state.
	 *
	 * Central icon is set to 'base' icon on STOP, 'base'+"c" on CLOSING, 'base'+"o" on OPENING. Eg.
	 * if 'base' icon path is "center.png", icon path "centerc.png" will be used on CLOSING. The same applies
	 * for OPENING.
	 * Alternate icon is set on left (respectively, right) button on CLOSING (OPENING)
	 */
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

/*
 * Banner with two central buttons.
 *
 * By default the two buttons are separated by a space, but they can be stuck
 * together and operate as the two halves of a larger button.
 */
class Bann2CentralButtons : public BannerNew
{
Q_OBJECT
protected:
	/*
	 * \param spaced_buttons add spacing between the two buttons
	 */
	Bann2CentralButtons(bool spaced_buttons = true);

	/*
	 * \param banner_text description text under the banner; if empty the label is deleted
	 */
	void initBanner(const QString &left, const QString &right, const QString &banner_text);

	BtButton *center_left, *center_right;
private:
	QLabel *text;
};


/*
 * Banner with a central LCD and two lateral buttons to change the value.
 *
 * This banner sends the signal valueChanged() after the user has stoped to use the controls.
 */
class BannLCDRange : public BannerNew
{
Q_OBJECT
public:
	BannLCDRange(QWidget *parent = 0);

	void setRange(int minimum, int maximum);

	void setValue(int value);
	int value() const;

	void setNumDigits(int n);

protected:
	void initBanner();

signals:
	void valueChanged(int value);

private slots:
	void plusClicked();
	void minusClicked();
	void emitValueChanged();

private:
	void startTimer();

	QTimer *control_timer;
	QLCDNumber *lcd;
	BtButton *minus;
	BtButton *plus;
	int min;
	int max;
};


/*!
	\ingroup Core
	\brief A base class for special banners that have a button on the left, a text
	in the center and must show an icon in the traybar, only if the status is on.

	To abilitate/disabilitate the tray icon, tou can use the toggleActivation()
	method.

	It is possible to reimplement the updateStatus() method to provide a custom
	behaviour. In the base class, this method simply shows or hides the tray
	icon depending on the button status.

	\sa StateButton, TrayBar
*/
class BannOnTray : public Bann2StateButtons
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new BannOnTray with the given \a icon_on as on icon,
		\a icon_off as off icon, \a tray_icon as tag for the tray icon, \a tray_id
		as button id, \a status as initial status and \a item_id as configuration id.
	*/
	BannOnTray(const QString &label, const QString &icon_on, const QString &icon_off,
		const QString &tray_icon, TrayBar::ButtonId tray_id, bool status=false, int item_id=-1);

private slots:
	/*!
		\brief Sets the status of the button.

		The status is reflected to the tray icon visibility and saved on the
		configuration.

		\sa updateStatus()
	*/
	void toggleActivation();

protected:
	/*!
		\brief Updates the status of the button.

		The default implementation shows or hides the tray icon depending on
		the button status.

		\sa StateButton
	*/
	virtual void updateStatus();
	/*!
		\brief The tray button (used as a simply icon).
	*/
	BtButton *tray_button;

private:
	int item_id;
};

#endif // BANN2_BUTTONS_H
