/*!
 * \banntemperature.h
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief 
 *
 * 
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNTEMPERATURE_H
#define BANNTEMPERATURE_H

#include "banner.h"
#include "device.h"
#include "qdom.h"

#define DESCR_LABEL_WIDTH 180
class QLabel;

class BannTemperature : public banner
{
Q_OBJECT
public:
	BannTemperature(QWidget *, const char *, QDomNode);
	virtual void Draw();
public slots:
	void status_changed(QPtrList<device_status>);
private:
	/**
	 * \brief Dummy function to avoid instantiation 
	 *
	 * BannTemperature has no address so it shouldn't be possible to
	 * instatiate.
	 */
	virtual void dummy() = 0;
	QDomNode conf_root;
	QString  probe_descr;
	QString  temperature;
	QLabel  *temp_label;
	QLabel  *descr_label;
};

#endif // BANNTEMPERATURE_H

