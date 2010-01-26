#include "multimedia.h"
#include "multimedia_filelist.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "main.h" // MULTIMEDIA
#include "skinmanager.h"
#include "webcam.h"

enum
{
	PAGE_USB = 2600,
	PAGE_LAN = 2601,
	PAGE_WEB_RADIO = 2603,
	PAGE_WEB_TV = 2604,
	PAGE_WEB_CAM = 2605,
	PAGE_RSS = 2606,
	PAGE_SD = 2609,
};


MultimediaSectionPage::MultimediaSectionPage(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int MultimediaSectionPage::sectionId()
{
	return MULTIMEDIA;
}

void MultimediaSectionPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int page_id = getTextChild(page_node, "id").toInt();
		int item_id = getTextChild(item, "id").toInt();

		Page *p = 0;

		// use the item_id for now because some of the items do not
		// have a linked page
		switch (item_id)
		{
		case PAGE_USB:
			p = new MultimediaFileListPage;
			break;
		case PAGE_WEB_CAM:
			p = new WebcamListPage(page_node);
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
		};

		if (p)
		{
			p->inizializza();
			addPage(p, descr, icon);
		}
	}
}
