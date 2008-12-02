/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Video-EntryPhone section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef VIDEOENTRYPHONE_H
#define VIDEOENTRYPHONE_H

#include "sottomenu.h"

#include <QWidget>

class QDomNode;


class VideoEntryPhone : public sottoMenu
{
Q_OBJECT
public:
	VideoEntryPhone(QWidget *parent, QDomNode config_node);

private:
	void loadDevices(QDomNode config_node);
};

#endif
