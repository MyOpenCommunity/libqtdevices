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


class PlantMenu : public BannerPage
{
Q_OBJECT
public:
	PlantMenu(const QDomNode &conf);
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &conf);

	/**
	 * Utility function to create a banner in the plant menu and the corresponding full
	 * screen banner.
	 * \param n             The node in the configuration DOM which is a `item' tag
	 * \param central_icon  The path of the icon to be shown in plant menu banner
	 * \param type          The type of full screen banner that is linked to the banner in plant menu
	 * \return A pointer to the small banner that will show the corresponding full screen banner.
	 */
	NavigationPage *addMenuItem(QDomNode n, QString central_icon, BannID type);

	/// Address of the thermal regulation device
	/// empty in case of 99z device
	QString ind_centrale;
};

#endif // PLANTMENU_H
