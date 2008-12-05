/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** sottomenu.h
 **
 **definizioni della pagine di sottoMenu
 **
 ****************************************************************/

#ifndef SOTTOMENU_H
#define SOTTOMENU_H

#include "main.h"
#include "banner.h"
#include "device_status.h"
#include "bttime.h"
#include "btwidget.h"

#include <QDomNode>
#include <QDateTime>


class bannFrecce;
class scenEvo_cond;
class FSBannTime;
class FSBannDate;

/*!
  \class sottoMenu
  \brief This class is the general list of the subtree describing the various system functions.

  Many functions are directily a \a sottoMenu type while other are new class inheriting this one. This class core is made of a dinamical list representing all the devices included in the menu.
  \author Davide
  \date lug 2005
  */
class sottoMenu : public BTWidget
{
Q_OBJECT
public:
	sottoMenu(QWidget *parent=0, uchar withNavBar=3 ,int width=MAX_WIDTH,int  height=MAX_HEIGHT,uchar n=NUM_RIGHE-1);
	~sottoMenu();

	/**
	* Add a new banner.
	*/
	void appendBanner(banner *b);

	/*!
	\brief Initializes all the objects in the list calling init() after a certain time
	*/
	virtual void inizializza();

	/*!
	\brief Draws the subtree starting from the right item.

	The drawn isn't made if the first item to draw doesn't change
	*/
	void draw();

	/*!
	\brief Draws the subtree starting from the right item.

	The drawn is always made
	*/
	void forceDraw();

	/*!
	\brief Set the row's number to be drawn in the screen
	*/
	virtual void setNumRighe(uchar);

	/**
	* Set the number of rows to scroll up or down at each goUp() or goDown() call
	* \param step The number of rows to scroll up or down
	*/
	void setScrollStep(unsigned step);

	/*!
	\brief Retrieves the last banner of the list
	*/
	banner* getLast();

	/*!
	\brief Retrieves the number of rows used by the subtree.
	*/
	uchar getNumRig();

	/*!
	\brief Changes the type of navigation bar present at the bsubtree (see bannFrecce)
	*/
	virtual void setNavBarMode(uchar=0, QString IconBut4=ICON_FRECCIA_DX);

	/*!
	\brief Set the Geometry for the object.
	*/
	void setGeometry(int, int, int, int);

	/*!
	\brief reparent implementation
	*/
	virtual void reparent(QWidget * parent, Qt::WindowFlags f, const QPoint & p, bool showIt = FALSE);

public slots:
	void showItem(int id);

protected:
	void connectLastBanner();

	virtual void hideEvent(QHideEvent *event);
	virtual void showEvent(QShowEvent *event);
	QList<banner*> elencoBanner;
	int indice, indicold;
	unsigned int height,width;
	uchar numRighe, hasNavBar;
	bannFrecce * bannNavigazione;
	QString iconName;

protected slots:
	/*!
	\brief Slides the list downward.
	*/
	virtual void goDown();

private:
	/// Number of banners to scroll at each goUp() or goDown() call. Default value is 1, to avoid
	/// breaking existing code.
	unsigned scroll_step;
	void setModeIcon(QString iconBut4);

private slots:
	/*!
	\brief Slides the list upward.
	*/
	virtual void goUp();

	/*!
	\brief  See inizializza().
	*/
	void init();

signals:
	void goUP();
	void goDO();

	/*!
	\brief Emitted to connect the \a Open \a Frame coming from the openClient to the banners in the list.
	*/
	void gestFrame(char*);

	/*!
	\brief Open ack received
	*/
	void openAckRx(void);

	/*!
	\brief Open nak received
	*/
	void openNakRx(void);

	void richStato(QString);
	void rispStato(char*);

	/*!
	\brief Emitted to go to a further page.
	*/
	void goDx();

	/*!
	\brief Emitted to tell that an item in the list was removed.
	*/
	void itemKilled();

	/*!
	\brief Emitted to tell to BtMain tha state of the password (value and abilitation).
	*/
	void setPwd(bool,QString);

	/*!
	\brief Parent changed
	*/
	void parentChanged(QWidget *newParent);

	/*!
	\brief hide all children
	*/
	void hideChildren();

	/*!
	\brief emitted on calibration start
	*/
	void startCalib();

	/*!
	\brief emitted on calibration end
	*/
	void endCalib();
};


/**
 * A base class for submenus that allow to choose one program in a list. The list changes
 * when season changes (summer/winter).
 * This class emits a signal when a program is clicked. This signal should be used to close
 * the submenu and to take further action, for example sending a frame to the thermal regulator.
 */
class ProgramMenu : public sottoMenu
{
Q_OBJECT
public:
	ProgramMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners() = 0;
	virtual void createWinterBanners() = 0;
public slots:
	void status_changed(QList<device_status*> sl);
protected:
	int season;
	QDomNode conf_root;
signals:
	void programClicked(int);
};

/**
 * This is a specialized version of ProgramMenu to select week programs. The list
 * of programs is read from DOM.
 */
class WeeklyMenu : public ProgramMenu
{
Q_OBJECT
public:
	WeeklyMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

/**
 * This is a specialized version of ProgramMenu to select scenarios. The list
 * of scenarios is read from DOM and updated when season changes
 */
class ScenarioMenu : public ProgramMenu
{
Q_OBJECT
public:
	ScenarioMenu(QWidget *parent, QDomNode conf);
	virtual void createSummerBanners();
	virtual void createWinterBanners();
};

/**
 * A submenu that let the user choose the time.
 */
class TimeEditMenu : public sottoMenu
{
Q_OBJECT
public:
	TimeEditMenu(QWidget *parent=0);
	BtTime time();
private:
	FSBannTime *time_edit;
private slots:
	void performAction();
signals:
	void timeSelected(BtTime);
};

/**
 * A submenu that let the users choose a date and emits a signal with the selected date when the user
 * confirms the choice.
 */
class DateEditMenu : public sottoMenu
{
Q_OBJECT
public:
	DateEditMenu(QWidget *parent=0);
	QDate date();
private:
	FSBannDate *date_edit;
private slots:
	void performAction();
signals:
	void dateSelected(QDate);
};

#endif // SOTTOMENU_H
