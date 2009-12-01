#include "iconsettings.h"
#include "changedatetime.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "skinmanager.h"
#include "btmain.h" // version page
#include "version.h"
#include "screensaverpage.h"
#include "cleanscreen.h"
#include "btbutton.h"
#include "main.h" // pagSecLiv

enum
{
	ICON_BEEP = 2901,
	PAGE_DATE_TIME = 2902,
	PAGE_BRIGHTNESS = 2903,
	PAGE_PASSWORD = 2907,
	PAGE_VOLUME = 2908,
	PAGE_RINGTONES = 2909,
	PAGE_CALIBRATION = 2910,
	PAGE_VERSION = 2911,
	PAGE_SCREENSAVER = 2912,
	PAGE_CLEANSCREEN = 2913,
	PAGE_DISPLAY = 2920,
	PAGE_ALARMCLOCK = 2950
};


IconSettings::IconSettings(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int IconSettings::sectionId()
{
	return IMPOSTAZIONI;
}

void IconSettings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int link_id = getTextChild(item, "id").toInt();
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");
		Page *p = 0;
		Window *w = 0;

		switch (link_id)
		{
		case PAGE_DATE_TIME:
			p = new ChangeTime;
			break;
		case PAGE_DISPLAY:
			p = new IconSettings(getPageNodeFromPageId(getTextChild(item, "lnk_pageID").toInt()));
			break;
		case PAGE_SCREENSAVER:
			p = new ScreenSaverPage;
			break;
		case PAGE_CLEANSCREEN:
			// TODO config file does not contain the clean screen value
			w = new CleanScreen(bt_global::skin->getImage("cleanscreen"), 10);
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
		};

		if (p)
			addPage(p, link_id, descr, icon);
		else if (w)
		{
			BtButton *b = addButton(link_id, descr, icon);

			connect(b, SIGNAL(clicked()), w, SLOT(showWindow()));
		}
	}
}
