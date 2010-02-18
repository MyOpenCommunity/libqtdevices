/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann4_buttons.h
**
** In this file there are contained all generic banners with 4 buttons.
**
****************************************************************/

#ifndef BANN4_BUTTONS_H
#define BANN4_BUTTONS_H

#include "banner.h"

class BtButton;
class QWidget;
class QLabel;

// substitute for bann4tasLab
class Bann4ButtonsIcon : public BannerNew
{
Q_OBJECT
protected:
	enum States
	{
		LOCKED,               // device is locked, no edit is possible (no right button visible)
		UNLOCKED,             // scenario edit is possible (pencil icon on right)
		EDIT_VIEW,            // edit buttons visible (right arrow icon on right)
	};
	enum EditingStates
	{
		EDIT_ACTIVE,          // editing is enabled (stop icon on left)
		EDIT_INACTIVE,        // editing is disabled (on icon on left)
	};

	Bann4ButtonsIcon(QWidget *parent);
	void initBanner(const QString &_right, const QString &_right_alt, const QString &_center,
		const QString &_c_left, const QString &_c_right, const QString &_left,
		const QString &_left_alt, States init_state, const QString &banner_text);
	void setState(States new_state);
	void setEditingState(EditingStates edit_state);
	BtButton *left_button, *center_left_button, *center_right_button, *right_button;

private:
	QLabel *center_icon, *text;
	QString left_icon, left_alt, right_icon, right_alt;
};

// substitute for bann4But
class Bann4Buttons : public BannerNew
{
Q_OBJECT
protected:
	Bann4Buttons(QWidget *parent);
	void initBanner(const QString &right, const QString &center_right, const QString &center_left,
		const QString &left, const QString &banner_text);

	BtButton *right_button, *center_right_button, *center_left_button, *left_button;
private:
	QLabel *text;
};


/*!
  \class bann4But
  \brief This is a class that describes a banner with 4 buttons
  \author Ciminaghi
  \date June 2006
*/
class bann4But : public BannerOld
{
Q_OBJECT
public:
	bann4But(QWidget *);
};


/*!
\class bann4tasLab
\brief This is a class that describes a banner with 4 buttons a label and a text under them.
In this banner it is possible to have 4 buttons visible and the label hidden or two external buttons visible with the label and the central two buttons hidden
\author Davide
\date lug 2005
*/
class bann4tasLab : public BannerOld
{
Q_OBJECT
public:
	bann4tasLab(QWidget * parent=NULL);
};

#endif // BANN4_BUTTONS_H
