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

class QLabel;


/*!
  \class Aux
  \brief This class implements the management of the aux source page

  \author Ciminaghi
  \date lug 2006
  */
class Aux : public Page
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
	QLabel *amb_descr;
};


#endif // AUX_H
