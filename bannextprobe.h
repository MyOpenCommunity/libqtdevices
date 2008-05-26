/*!
 * \bannextprobe.h
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
#ifndef BANNEXTPROBE_H
#define BANNEXTPROBE_H

#include "banntemperature.h"

class BannExtProbe : public BannTemperature
{
Q_OBJECT
public:
	BannExtProbe(QWidget *, const char *, QDomNode);
private:
	virtual void dummy() { };	
};
#endif // BANNEXTPROBE_H
