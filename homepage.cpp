/****************************************************************
***
* BTicino Touch scren Colori art. H4686
**
** homePage.cpp
**
** Finestra principale
**
****************************************************************/

#include "homepage.h"
#include "timescript.h"
#include "fontmanager.h"
#include "temperatureviewer.h"
#include "btbutton.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "hardware_functions.h"

#include <QDomNode>


homePage::homePage(const QDomNode &config_node) : SectionPageContainer(config_node)
{
	temp_viewer = new TemperatureViewer(this);
	loadItems(config_node);
}

// Load only the item that is not a section page (which is loaded by SectionPageContainer)
void homePage::loadItems(const QDomNode &config_node)
{
	if (hardwareType() == TOUCH_X)
		return;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		switch (id)
		{
		case DATA:
		case OROLOGIO:
		{
			timeScript *d = new timeScript(this, id == DATA ? 25 : 1);
			d->setGeometry(x + 10, y + 10, 220, 60);
			d->setLineWidth(3);
			break;
		}
		case TEMPERATURA:
		case TERMO_HOME_NC_PROBE:
			temp_viewer->add(getTextChild(item, "where"), x, y, 220, 60, "", "0");
			break;
		case TERMO_HOME_NC_EXTPROBE:
			temp_viewer->add(getTextChild(item, "where"), x, y, 220, 60, "", "1");
			break;
		}
	}
}

void homePage::inizializza()
{
	temp_viewer->inizializza();
}

Page::PageType homePage::pageType()
{
	return HOMEPAGE;
}

