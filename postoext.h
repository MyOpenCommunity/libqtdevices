/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** postoext.h
 **
 **
 **
 ****************************************************************/

#ifndef POSTOEXT_H
#define POSTOEXT_H

#include "bann4taslab.h"

#include <qwidget.h>
#include <qstring.h>

/// Forward Declarations
class dati_sorgente_multi;
class call_notifier_manager;
class call_notifier;


/*****************************************************************
 ** Posto esterno
 ****************************************************************/
/*!
 * \class postoExt
 * \brief This class represents an outdoor station
 * \author Ciminaghi
 * \date apr 2006
 */
class postoExt : public bann4tasLab
{
Q_OBJECT
private:
	QString where;
	QString descr;
	bool light;
	bool key;
	bool unknown;
	QString light_icon;
	QString key_icon;
	QString close_icon;
	static call_notifier_manager *cnm;
	static call_notifier *unknown_notifier;
public:
	postoExt(QWidget *parent=0, const char *name=NULL, char* Icona1="",char *Icona2="", char *Icona3="", char* Icona4="", char *where="", char *light="", char *key="", char *unknown = "0");
	//! Read where
	void get_where(QString&);
	//! Read description
	void get_descr(QString&);
	//! Get key mode
	bool get_key(void);
	//! Get light mode
	bool get_light(void);
	//! Get light icon name
	void get_light_icon(QString&);
	//! Get key icon name
	void get_key_icon(QString&);
	//! Cancel icon name
	void get_close_icon(QString&);
public  slots:
	//! Invoked when right button is pressed
	void stairlight_pressed(void);
	//! Invoked when right button is released 
	void stairlight_released(void); 
	//! Invoked when left button is clicked
	void open_door_clicked(void);
	//! Usual gestFrame
	void gestFrame(char *);
	//! Invoked when a frame has been captured by a call_notifier
	void frame_captured_handler(call_notifier *);
	//! Invoked when a call_notifier window is closed
	void call_notifier_closed(call_notifier *);
signals:
	//! Emitted when a frame is available
	void frame_available(char *);
};

#endif
