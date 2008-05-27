/*!
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "header_name.h"


PlantMenu::PlantMenu(QWidget *parent, char *name, QDomNode conf)
	: sottoMenu(parent, name)
{
	conf_root = conf;
	ind_centrale = conf_root.namedItem("ind_centrale").toElement().text();
	
	QDomNode n = conf_root.firstChild(); //??
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			//QDomNode item = findNamedNode(n, "descr");
			//QString descr = item.toElement().text();
			banner *b = new bannPuls(sm, "");
			qDebug("[TERMO] createPlantMenu: item = %s", n.nodeName().ascii());

			initBanner(b, n);

			QString leftIcon(IMG_PATH + QString(I_RIGHT_ARROW));
			QString central_icon;
			int id = n.namedItem("id").toElement().text().toInt();
			if (id == TERMO || id == TERMO_FANCOIL)
				central_icon = QString(IMG_PATH) + I_TEMP_PROBE;
			else
				central_icon = QString(IMG_PATH) + I_PLANT;
			b->SetIcons(leftIcon.ascii(), 0, central_icon.ascii());


			elencoBanner.append(b);
		}
		n = n.nextSibling();
	}

}
