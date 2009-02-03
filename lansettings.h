/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the page to show and control network settings.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef LAN_SETTINGS_H
#define LAN_SETTINGS_H

#include "page.h"

#include <QStringList>
#include <QVariant>
#include <QHash>

class LanDevice;
class BtButton;
class QDomNode;
class QLabel;


/**
 * \class LanSettings
 *
 * This class is a page used to show lan settings and enable activation
 * (disactivation) of lan interface.
 */
class LanSettings : public Page
{
Q_OBJECT
public:
	LanSettings(const QDomNode &config_node);
	virtual void inizializza();

private:
	BtButton *toggle_btn;
	QLabel *box_text;
	LanDevice *dev;
	QStringList text;
	bool lan_status;

private slots:
	void status_changed(StatusList status_list);
	void toggleLan();
};

#endif // LAN_SETTINGS_H
