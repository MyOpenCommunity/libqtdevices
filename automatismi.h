/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** automatismi.h
 **
 **definizioni dei vari automatismi implementati
 **
 ****************************************************************/

#ifndef AUTOMATISMI_H
#define AUTOMATISMI_H

#include "bannbuticon.h"

#include <qwidget.h>
#include <qstring.h>

/// Forward Declarations
class device;

/*****************************************************************
 ** Automatismi-Cancello con attuatore videocitofonia 
 ****************************************************************/
/*!
 * \class
 * \brief This class represents an automated video-doorphone actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatVC : public bannButIcon
{
Q_OBJECT
private:
	device *dev;
public:
	automCancAttuatVC(QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL);
private slots:
	void Attiva();
};


/*****************************************************************
 ** Automatismi-Cancello con attuatore ILLUMINAZIONE
 ****************************************************************/
/*!
 * \class 
 * \brief This class represents an automated light actuator
 * \author Ciminaghi
 * \date June 2006
 */
class automCancAttuatIll : public bannButIcon
{
Q_OBJECT
private:
	device *dev;
	QString time;
public:
	automCancAttuatIll(QWidget *parent=0, const char *name=NULL ,char*indirizzo=NULL,char* IconaSx=NULL,char* IconaDx=NULL, QString t = QString());
private slots:
	void Attiva();
};

#endif
