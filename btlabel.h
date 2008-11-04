#ifndef BTLABEL_H
#define BTLABEL_H

/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements a label with the "btstyle"
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#include <QLabel>


class BtLabel : public QLabel
{
Q_OBJECT
public:
	BtLabel(QWidget *parent);
};

#endif
