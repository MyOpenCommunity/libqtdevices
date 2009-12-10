#include "sectionpage.h"
#include "hardware_functions.h" // rearmWDT, hardwareType, TOUCH_X
#include "xml_functions.h" // getTextChild, getChildren
#include "navigation_bar.h"
#include "main.h" // IMG_PATH
#include "pagefactory.h" // getPage
#include "skinmanager.h"

#include <QTime>
#include <QDomNode>


SectionPage::SectionPage(const QDomNode &config_node)
{
	loadItems(config_node);
}

void SectionPage::loadItems(const QDomNode &config_node)
{
#ifndef CONFIG_BTOUCH
	// only show the back icon if there is a lnk_itemID to a banner
	QString back_icon = getTextChild(config_node, "lnk_itemID").isEmpty() ? "" : "back";
	NavigationBar *nav_bar = new NavigationBar("", "scroll_down", "scroll_up", back_icon);
	buildPage(new IconContent, nav_bar);
#endif

	QTime wdtime;
	wdtime.start(); // Start counting for wd refresh

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
#ifdef CONFIG_BTOUCH
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		// Within the pagemenu element, it can exists items that are not a page.
		if (Page *p = getPage(id))
			addPage(p, id, QString(), img1, x, y);
#else
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		int link_id = getTextChild(item, "id").toInt();
		// Home page buttons don't have description set
		QString descr = getTextChild(item, "descr");

		// TODO some ids are not links
		int pageid = getTextChild(item, "lnk_pageID").toInt();
		if (Page *p = getPage(pageid))
			addPage(p, pageid, descr, icon, 0, 0);
#endif

		if (wdtime.elapsed() > 1000)
		{
			wdtime.restart();
			rearmWDT();
		}
	}
}

