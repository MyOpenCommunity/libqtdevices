/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef PLANTMENU_H
#define PLANTMENU_H

#include "page.h"
#include "bann_thermal_regulation.h"

#include <QSignalMapper>
#include <QString>
#include <QDomNode>

class device;
class NavigationPage;


class PlantMenu : public Page
{
Q_OBJECT
public:
	PlantMenu(QWidget *parent, QDomNode conf);
	virtual void inizializza();

private:
	/**
	 * Utility function to create a banner in the plant menu and the corresponding full
	 * screen banner.
	 * \param n             The node in the configuration DOM which is a `item' tag
	 * \param central_icon  The path of the icon to be shown in plant menu banner
	 * \param descr         The text to be shown in the banner in plant menu
	 * \param type          The type of full screen banner that is linked to the banner in plant menu
	 * \param addr          The address of the device observed by the banner
	 * \param dev           A pointer to the device observed by the banner
	 * \param bannfullscreen The full screen banner that is created with the call to \a addMenuItem
	 * \return A pointer to the small banner that will show the corresponding full screen banner.
	 */
	NavigationPage *addMenuItem(QDomNode n, QString central_icon, QString descr, BannID type);

	/// Node in the Dom tree that is a `plant' tag
	QDomNode conf_root;

	/// Address of the thermal regulation device
	/// empty in case of 99z device
	QString ind_centrale;
};

#endif // PLANTMENU_H
