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
#include "page.h"

#include <QDomNode>
#include <QDateTime>

class banner;
class bannFrecce;
class scenEvo_cond;

/*!
  \class sottoMenu
  \brief This class is the general list of the subtree describing the various system functions.

  Many functions are directily a \a sottoMenu type while other are new class inheriting this one. This class core is made of a dinamical list representing all the devices included in the menu.
  \author Davide
  \date lug 2005
  */
class sottoMenu : public Page
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

	/**
	 * Reset the index of the top banner to be shown
	 */
	void resetIndex();

protected:

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
	/*!
	\brief Open ack received
	*/
	void openAckRx();

	/*!
	\brief Open nak received
	*/
	void openNakRx();

	/*!
	\brief Emitted to go to a further page.
	*/
	void goDx();

	/*!
	\brief Parent changed
	*/
	void parentChanged(QWidget *newParent);
};

#endif // SOTTOMENU_H
