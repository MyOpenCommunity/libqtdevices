/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** bann1_button.h
**
** In this file there are contained all generic banners with 1 button.
**
****************************************************************/

#ifndef BANN1_BUTTON_H
#define BANN1_BUTTON_H

#include "banner.h"

#include <QList>
#include <QPair>

class BtButton;
class Page;
class QWidget;
class TextOnImageLabel;
class QLabel;


#define BANPULS_BUT_DIM  60
#define BANPULS_ICON_DIM_X 120
#define BANPULS_ICON_DIM_Y 60


// substitute for bannPuls
class BannSinglePuls : public BannerNew
{
Q_OBJECT
public:
	BannSinglePuls(QWidget *parent);
	void initBanner(const QString &right, const QString &center, const QString &banner_text);
	void connectRightButton(Page *p);

signals:
	void rightClick();

protected:
	BtButton *right_button;

private:
	void loadIcons(const QString &right, const QString &center);
	QLabel *center_icon, *text;
};


class BannOn2Labels : public BannerNew
{
Q_OBJECT
protected:
	enum States
	{
		ON,
		OFF,
	};
	BannOn2Labels(QWidget *parent);
	void initBanner(const QString &right, const QString &_right_icon, const QString &_left_icon,
		const QString &banner_text, const QString &_center_text);
	// 1 <= time <= 8
	void setElapsedTime(int time);
	void setState(States new_state);
	BtButton *right_button;

private:
	QString center_right, center_left;
	QLabel *left_icon, *right_icon, *text, *center_text;
};


// Single button on the center, without bottom label
class BannCenteredButton : public BannerNew
{
Q_OBJECT
public:
	void connectButton(Page *linked_page);

protected:
	BannCenteredButton(QWidget *parent);
	void initBanner(const QString &center);

	BtButton *center_button;
};


class BannSimple : public BannCenteredButton
{
Q_OBJECT
public:
	BannSimple(const QString &icon);

signals:
	void clicked();
};


/*!
  \class bannPuls
  \brief This is a class that describes a banner with a button on the right, an icon in the center and a text on the bottom
  \author Davide
  \date lug 2005
*/
class bannPuls : public BannerOld
{
Q_OBJECT
public:
	bannPuls(QWidget *parent);

signals:
	void click();
	void pressed();
	void released();
};


/*!
  \class bannOnDx
  \brief This is a class that describes a banner with a button on the right and a text on the remaining area
  \author Davide
  \date lug 2005
*/
class bannOnDx : public BannerOld
{
Q_OBJECT
public:
	bannOnDx(QWidget *parent, QString icon=QString(), Page *page=0);

signals:
	void click();
};


/*!
  \class bannOnSx
  \brief This is a class that describes a banner with a button on the left and a text on the remaining area
  \author Davide
  \date lug 2005
*/
class bannOnSx : public BannerOld
{
Q_OBJECT
public:
	bannOnSx(QWidget *parent, QString icon=QString());
	BtButton *getButton();
signals:
	void click();
};



class bannOnIcons : public BannerOld
{
Q_OBJECT
public:
	bannOnIcons(QWidget *parent);
};


/*!
  \class bannBut2Icon
  \brief This is a class that describes a banner with a button on the right,
  two icons in the middle and some text below
  \author Ciminaghi
  \date jun 2006
*/
class bannBut2Icon : public BannerOld
{
Q_OBJECT
public:
	bannBut2Icon(QWidget *);
};


class bannTextOnImage : public BannerOld
{
Q_OBJECT
public:
	bannTextOnImage(QWidget *parent, const QString &text = QString(), QString bg_image = "empty_icon", QString fwd_image = "forward");
	void setInternalText(const QString &text);

private:
	TextOnImageLabel *label;
};


/**
 * A banner that encapsulates a list of states that can be cycled using a button
 * on the left. Every state has an id and a description.
 */
class BannStates : public BannerNew
{
Q_OBJECT
public:
	BannStates(QWidget *parent);
	void addState(int id, QString descr);

	// Init the banner [NOTE: must be called after addState]
	void initBanner(const QString &left, int current_state);

	// Return the id of the current state
	int currentState();

	// Set a different state. The state must be previously added with addState().
	// You can use states given from currentState().
	void setCurrentState(int new_state);

protected:
	BtButton *left_button;

private:
	void updateText();
	int current_index;
	QLabel *text;
	QList<QPair<int, QString> > states_list;

private slots:
	void changeState();
};


#endif // BANN1_BUTTON_H
