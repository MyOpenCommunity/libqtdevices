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

};


#endif // AUX_H
