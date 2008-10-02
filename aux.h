/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** aux.h
 **
 **definizioni della pagina di visualizzazione aux
 **
 ****************************************************************/

#ifndef AUX_H
#define AUX_H

#include <qwidget.h>
#include <qstring.h>

class BtLabel;
class BtButton;
class bannFrecce;

/*!
  \class aux
  \brief This class implements the management of the aux source page

  \author Ciminaghi
  \date lug 2006
  */
class  aux : public QWidget
{
Q_OBJECT
public:
	aux(QWidget *parent=0, const QString & name="", const QString & amb="");

	/*!
	\brief Sets the background color for the banner.

	The arguments are RGB components for the color.
	*/
	void setBGColor(int, int , int);

	/*!
	\brief Sets the foreground color for the banner.

	The arguments are RGB components for the color.
	*/
	void setFGColor(int , int , int);

	/*!
	\brief Sets the background color for the banner.

	The argument is the QColor description of the color.
	*/
	void setBGColor(QColor);

	/*!
	\brief Sets the foreground color for the banner.

	The argument is the QColor description of the color.
	*/
	void setFGColor(QColor);

	BtLabel* auxName, *ambDescr;
	BtButton *fwdBut;

	/*!
	\brief Sets amb. description
	*/
	void setAmbDescr(const QString &);

	/*!
	\brief Draws the page
	*/
	void draw();

signals:
	/*!
	\brief Emitted when the page is going to be closed
	*/
	void Closed();

	/*!
	\brief Emitted when fwd button is pressed
	*/
	void Btnfwd();

public slots:
	/*!
	\brief Shows the aux details page
	*/
	void showAux();

	/*!
	\brief Disables/enables everything
	*/
	void freezed(bool);

private:
	char amb[80];
	char nome[15];
	bannFrecce * bannNavigazione;

	// TODO: rimuovere questi metodi qt3!
	void setPaletteForegroundColor(const QColor &c);
	void setPaletteBackgroundColor(const QColor &c);
	void setPaletteBackgroundPixmap(const QPixmap &pixmap);
};


#endif // AUX_H
