/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** genpage.h
**
**definizioni della pagine di visualizzazione versioni
**
****************************************************************/
#ifndef  GENERIC_PAGES_H
#define GENERIC_PAGES_H

#include <QLabel>

/*!
  \class genPage
  \brief This class genertes a page as described in \a pageType. 

  \author Davide
  \date lug 2005
*/

class genPage : public QLabel
{
Q_OBJECT
public:
	genPage(QWidget *parent= NULL, unsigned char tipo=0, const char *img = NULL,unsigned int f =  0);
	enum pageType
	{
		RED = 0,  /*! \enum generates a \a red page */
		GREEN = 1,  /*! \enum generates a \a green page */
		BLUE = 2,  /*! \enum generates a \a blue page */
		IMAGE = 3,  /*! \enum generates a page which shows an image whose fileName is the forth argument of the constructor */
		NONE = 4
	};
};

#endif //GENERIC_PAGES_H
