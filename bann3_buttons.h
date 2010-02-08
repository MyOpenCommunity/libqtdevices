/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann3_buttons.h
**
** In this file there are contained all generic banners with 3 buttons.
**
****************************************************************/

#ifndef BANN3_BUTTONS_H
#define BANN3_BUTTONS_H

#include "banner.h"

class QWidget;

class Bann3Buttons : public BannerNew
{
Q_OBJECT
protected:
	Bann3Buttons(QWidget *parent);
	void initBanner(const QString &left, const QString &center, const QString &right,
		const QString &banner_text);
	BtButton *right_button, *center_button, *left_button;

private:
	QLabel *text;
};


/**
 * A banner with 2 (optional) buttons on both sides, a button + a label on the center, description below
 */
class Bann3ButtonsLabel : public BannerNew
{
Q_OBJECT
protected:
	enum State
	{
		DISABLED,
		ENABLED,
	};

	enum Forced
	{
		NOT_FORCED,
		FORCED,
	};

	Bann3ButtonsLabel(QWidget *parent=0);
	void initBanner(const QString &_left_forced, const QString &_left_not_forced, const QString &center_but, const QString &center_lab,
		const QString &right, State init_state, Forced init_forced, const QString &banner_text);
	void setState(State new_state);
	void setForced(Forced is_forced);

	// TODO: add states (active, not active...)
	BtButton *right_button, *center_button, *left_button;

private:
	QLabel *text, *center_label;
	QString left_forced, left_not_forced;
};

/*!
  \class bann3But
  \brief This is a class that describes a banner with a text between 2 buttons 
  \author Davide
  \date lug 2005
*/
class bann3But : public banner
{
Q_OBJECT
public:
	bann3But(QWidget *parent);

signals:
	void centerClick();
};


/*!
  \class bann3ButLab
  \brief This is a class that describes a banner with a label between 2 buttons and a text under them
  There are 2 alternative buttons on the left and only one is visible at a time.
  \author Davide
  \date lug 2005
*/
class bann3ButLab : public banner
{
Q_OBJECT
public:
	bann3ButLab(QWidget *parent);
};


#endif // BANN3_BUTTONS_H
