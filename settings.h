/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of the Settings section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date December 2008
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "sottomenu.h"

#include <QWidget>

class QDomNode;


class Settings : public sottoMenu
{
Q_OBJECT
public:
	Settings(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};

#endif
