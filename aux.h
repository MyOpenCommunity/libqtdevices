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

#include "page.h"

class QString;
class QLabel;
class BtButton;


/*!
  \class Aux
  \brief This class implements the management of the aux source page

  \author Ciminaghi
  \date lug 2006
  */
class Aux : public PageLayout
{
Q_OBJECT
public:
	Aux(const QString &name, const QString &amb);
	void setAmbDescr(const QString &);

signals:
	/*!
	\brief Emitted when fwd button is pressed
	*/
	void Btnfwd();

private:
	QLabel *aux_name, *amb_descr;
	BtButton *fwd_but;
};


#endif // AUX_H
