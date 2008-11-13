/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** carico.h
 **
 **
 **
 ****************************************************************/

#ifndef CARICO_H
#define CARICO_H

#include "bannondx.h" // class bannOnSx

class device;

/*****************************************************************
 **carico
 ****************************************************************/
/*!
 * \class carico
 * \brief This class is made to force an appliance.
 *
 * \author Davide
 * \date lug 2005
 */
class carico : public bannOnSx
{
Q_OBJECT
public:
	carico(sottoMenu *parent, const char *name, char *indirizzo, QString IconaSx);
	void inizializza(bool forza = false);
private slots:
	void Attiva();
public slots:
	void gestFrame(char*);
private:
	device *dev;
};

#endif
