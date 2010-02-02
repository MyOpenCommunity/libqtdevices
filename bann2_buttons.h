/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann2_buttons.h
**
** In this file there are contained all generic banners with 2 buttons.
**
****************************************************************/

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
class BannOnOffNew : public Bann2LinkedPages
{
Q_OBJECT
public:
	BannOnOffNew(QWidget *parent);
	void initBanner(const QString &left, const QString &center, const QString &right, const QString &text);

protected:
	void setBannerText(const QString &str);
	void setInternalText(const QString &text);

	TextOnImageLabel *center_icon;
	QLabel *text;
	QString center;
};


/**
 * A class similar to the BannOnOffNew class that changes the state and thus the center icon.
 */
class BannOnOffState : public BannOnOffNew
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
};

/**
 * Two buttons on the sides + description in the center.
 * Either button can be removed by giving an empty string as the icon parameter to initBanner(). Each button
 * can also be linked to a page.
 * Nicer replacement for bann2but, will replace also bannOnDx, bannOnSx
 */
class Bann2Buttons : public Bann2LinkedPages
{
Q_OBJECT
protected:
	Bann2Buttons(QWidget *parent = 0);
	void initBanner(const QString &left, const QString &right, const QString &banner_text,
		FontManager::Type font_type = FontManager::TEXT);
	void setCentralText(const QString &t);

private:
	QLabel *text;
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
	void initBanner(const QString &left, const QString &right, const QString &banner_text);
	BtButton *center_left, *center_right;
private:
	QLabel *text;
};


/*!
  \class bann2But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \author Davide
  \date lug 2005
*/
class bann2But : public BannerOld
{
Q_OBJECT
public:
	bann2But(QWidget *parent);
};


/*!
  \class bann2ButLa
  \brief This is a class that describes a banner with a text between 2 buttons and a text on the bottom
  \author Davide
  \date lug 2005
*/
class bann2ButLab : public BannerOld
{
Q_OBJECT
public:
	bann2ButLab(QWidget *parent);
	void setAutoRepeat();
protected:
	virtual QSize sizeHint() const;
};

#endif // BANN2_BUTTONS_H
