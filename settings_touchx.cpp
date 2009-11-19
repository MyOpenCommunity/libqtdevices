#include "settings_touchx.h"
#include "changedatetime.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "skinmanager.h"
#include "btmain.h" // version page
#include "version.h"

enum
{
	PAGE_DATE_TIME = 2902,
	PAGE_VERSION = 2911
};


SettingsTouchX::SettingsTouchX(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
}

void SettingsTouchX::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int link_id = getTextChild(item, "id").toInt();
		QString icon = bt_global::skin->getImage("link_icon");
		Page *p = 0;

		switch (link_id)
		{
		case PAGE_DATE_TIME:
			p = new ChangeTime;
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
		};

		if (p)
			addPage(p, link_id, icon);
	}
}
