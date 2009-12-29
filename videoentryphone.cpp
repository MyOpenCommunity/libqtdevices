#include "videoentryphone.h"
#include "bann_videoentryphone.h"
#include "bann2_buttons.h"
#include "xml_functions.h" // getTextChild, getChildren
#include "bannercontent.h"
#include "main.h" // bt_global::config
#include "skinmanager.h"
#include "btbutton.h"
#include "navigation_bar.h"
#include "entryphone_device.h"
#include "devices_cache.h" // bt_global::add_device_to_cache

#include <QSignalMapper>
#include <QDebug>


#ifdef LAYOUT_BTOUCH
VideoEntryPhone::VideoEntryPhone(const QDomNode &config_node)
{
	buildPage();
	loadDevices(config_node);
}

void VideoEntryPhone::loadDevices(const QDomNode &config_node)
{
	QString unknown = getTextChild(config_node, "unknown");

	foreach (const QDomNode &device, getChildren(config_node, "device"))
	{
		int id = getTextChild(device, "id").toInt();
		if (id != POSTO_ESTERNO)
			qFatal("Type of device not handled by Video EntryPhone page!");
		QString img1 = IMG_PATH + getTextChild(device, "cimg1");
		QString img2 = IMG_PATH + getTextChild(device, "cimg2");
		QString img3 = IMG_PATH + getTextChild(device, "cimg3");
		QString img4 = IMG_PATH + getTextChild(device, "cimg4");
		QString descr = getTextChild(device, "descr");
		QString light = getTextChild(device, "light");
		QString key = getTextChild(device, "key");
		QString where = getTextChild(device, "where");

		banner *b = new postoExt(this, descr, img1, img2, img3, img4, where, light, key, unknown);
		b->setText(descr);
		b->setId(id);
		b->Draw();
		page_content->appendBanner(b);
	}
}
#else
VideoEntryPhone::VideoEntryPhone(const QDomNode &config_node) :
	SectionPage(config_node)
{
}

int VideoEntryPhone::sectionId()
{
	return VIDEOCITOFONIA;
}


CallExclusion::CallExclusion(const QDomNode &config_node)
{
	buildPage();
	SkinContext context(getTextChild(config_node, "cid").toInt());
	BannOnOffState *b = new BannOnOffState(0);
	b->initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("call_exclusion"),
		bt_global::skin->getImage("on"), BannOnOffState::ON, tr("Call exclusion"));
	page_content->appendBanner(b);
}


VideoControl::VideoControl(const QDomNode &config_node)
{
	// we must have only one entryphone device since we need to remember some state
	dev = bt_global::add_device_to_cache(new EntryphoneDevice(bt_global::config[PI_ADDRESS]));

	mapper = new QSignalMapper(this);
	buildPage(new IconContent, new NavigationBar);
	call_page = new VCTCallPage(dev);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"), 0, 0);
		connect(btn, SIGNAL(clicked()), mapper, SLOT(map()));

		// TODO: aggiungo 2 nell'ipotesi che videocontrol gestisca solo posti esterni..verificare!!
		QString where = "2" + getTextChild(item, "where");
		mapper->setMapping(btn, where);
	}

	connect(call_page, SIGNAL(Closed()), call_page, SLOT(showPreviousPage()));
	connect(mapper, SIGNAL(mapped(QString)), SLOT(cameraOn(QString)));
}

void VideoControl::cameraOn(QString where)
{
	dev->cameraOn(where);
}

void VideoControl::inizializza()
{
	dev->initVctProcess();
}


Intercom::Intercom(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"), 0, 0);
	}
}

#endif
