/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** versio.h
**
**definizioni della pagine di visualizzazione versioni
**
****************************************************************/
#ifndef  GENERIC_CLASSES_H
#define GENERIC_CLASSES_H

#include "qlabel.h"
#include "openclient.h"
#include "btlabel.h"

#include <qfont.h>
#include <stdlib.h>
#include <string.h>
/*!
  \class versio
  \brief This class shows the device versions.

  It shows \a software versione, \a pic version and \a hardware version and the seial number of the device among all the TouchScreens installed in the system.
  Software version is a serial number considering kernel, stack open and MMI. Pic version is the release of the SCS-interface micro-controller. Hw version is the PCB release (0=T7777A, 1=T7777B and so on). The serial number of the device is necessary to make diagnostic operation possible (making it watchable in this page facilitates configuration errors discovery)
  \author Davide
  \date lug 2005
*/  
class versio : public BtLabel
{
    Q_OBJECT
public:
     versio( QWidget *parent= NULL, const char *name = NULL, unsigned int f =  0);     
 /*!
  \brief Initialize the page asking the versions to the open stack.
*/       
     void   inizializza();
/*!
  \brief Sets the foreground color reimplementing QWidget::setPaletteForegroundColor( const QColor & )
*/                
     void   setPaletteForegroundColor( const QColor & );
/*!
  \brief Sets the foreground color reimplementing QWidget::setPaletteBackgroundColor( const QColor & )
*/        
     void   setPaletteBackgroundColor( const QColor & );
/*!
  \brief Sets the serial number of the device among all the TouchScreens installed in the system
*/        
     void 	setAddr(int);
/*!
  \brief Sets model string
*/
     void   setModel(const char *);

public slots:
/*!
  \brief Analyze the \a Open \a frame incoming to verify the versions to be shown
*/     
     void gestFrame(char*);
signals:
/*!
  \brief Emitted to send \a Open \a frame to ask which are the versions mounted on the device
*/ 
     void sendFrame(char*);	
     void	sendInit(char*);
private:
     unsigned char vers;
     unsigned char release;
     unsigned char build;
     unsigned char pic_version;
     unsigned char pic_release;
     unsigned char pic_build;
     unsigned char hw_version;
     unsigned char hw_release;
     unsigned char hw_build;
     QString model;
     BtLabel*datiGen, *bticino;
     int indDisp;
};

#endif //GENERIC_CLASSES_H
