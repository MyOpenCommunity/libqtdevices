/*!  * \bannsingleprobe.h * <!-- * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief
 *
 * 
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BANNSINGLEPROBE_H
#define BANNSINGLEPROBE_H

#include "banntemperature.h"

class BannSingleProbe : public BannTemperature
{
Q_OBJECT
public:
	BannSingleProbe(QWidget *, const char *, QDomNode);
private:
	virtual void dummy() { };
};

#endif // BANNSINGLEPROBE_H
