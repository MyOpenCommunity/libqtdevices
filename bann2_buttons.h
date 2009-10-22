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

class QWidget;
class QLabel;
class BtButton;


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
class BannOnOffState;
class BannOnOffNew : public BannerNew
{
// Avoid duplicating code between nearly identical classes. This class doesn't have states, so it simply
// displays the center icon provided. BannOnOffState can change states (and thus the center icon).
friend class BannOnOffState;
Q_OBJECT
protected:
	BannOnOffNew(QWidget *parent);
	void initBanner(const QString &left, const QString &center, const QString &right, const QString &text);
	void setBannerText(const QString &str);
	BtButton *left_button, *right_button;

private:
	void loadIcons(const QString &l, const QString &c, const QString &r);
	QLabel *center_icon;
	QLabel *text;
	QString left, center, right;
};

class BannOnOffState : public BannOnOffNew
{
Q_OBJECT
protected:
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

/*!
  \class bann2But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \author Davide
  \date lug 2005
*/
class bann2But : public banner
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
class bann2ButLab : public banner
{
Q_OBJECT
public:
	bann2ButLab(QWidget *parent);
	void setAutoRepeat();
protected:
	virtual QSize sizeHint() const;
};


/*!
  \class bannOnOff
  \brief This is a class that describes a banner with a button on the right and on the left, an icon on the center and a text on the bottom
  \author Davide
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
  \author Davide
  \date lug 2005
*/
class bannOnOff2scr : public banner
{
Q_OBJECT
public:
	bannOnOff2scr(QWidget *w=0);
};

#endif // BANN2_BUTTONS_H
