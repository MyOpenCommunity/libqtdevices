/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * This class implements a banner with on/off buttons on left/right margin
 * and a central icon with text that is shown on left, center or right depending
 * on its status.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef BANNONOFF_COMBO
#define BANNONOFF_COMBO

#include "banner.h"
#include <qwidget.h>

enum ComboStatus
{
	CENTER = 0,
	SX,
	DX,
	NUM_STATUS // fake status, used to get enum length
};


class BannOnOffCombo : public banner
{
	Q_OBJECT
public:
	BannOnOffCombo(QWidget *, const char *);

protected:
	void changeStatus(ComboStatus st);
	void SetIcons(const char *sxIcon , const char *dxIcon, const char *centerIcon, const char *centerSxIcon, const char *centerDxIcon);

private:
	QString status_icon[NUM_STATUS];
};

#endif
