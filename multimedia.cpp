#include "multimedia.h"
#include "multimedia_filelist.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "main.h" // MULTIMEDIA
#include "skinmanager.h"
#include "webcam_list.h"

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
		int link_id = getTextChild(item, "id").toInt();
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");
		Page *p = 0;

		switch (link_id)
		{
		case PAGE_USB:
			p = new MultimediaFileListPage;
			break;
		case PAGE_WEB_CAM:
			p = new WebcamListPage;
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
		};

		if (p)
		{
			p->inizializza();
			addPage(p, link_id, descr, icon);
		}
	}
}
