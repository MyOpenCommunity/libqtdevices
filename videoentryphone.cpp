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
#include "vctcall.h"
#include "generic_functions.h" //getBostikName
#include "items.h" // ItemTuning
#include "displaycontrol.h" // bt_global::display
#include "hardware_functions.h" // setVolume
#include "ringtonesmanager.h" // bt_global::ringtones

#include <QGridLayout>
#include <QSignalMapper>
#include <QDebug>
#include <QLabel>


enum Pages
{
	VIDEO_CONTROL=2200,  /*!< Video control menu */
	INTERCOM=2201,       /*!< Intercom menu */
	CALL_EXCLUSION=2203, /*!< Call exclusion */
};


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
VideoEntryPhone::VideoEntryPhone(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar);
	loadItems(config_node);
	dev = bt_global::add_device_to_cache(new EntryphoneDevice(bt_global::config[PI_ADDRESS]));
	connect(dev, SIGNAL(status_changed(StatusList)), SLOT(status_changed(StatusList)));
}

void VideoEntryPhone::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::RINGTONE:
			RingtoneType ringtone = static_cast<RingtoneType>(it.value().toInt());
			if (!call_exclusion->getStatus())
				bt_global::ringtones->playRingtone(ringtone);

			break;
		}
		++it;
	}
}

int VideoEntryPhone::sectionId()
{
	return VIDEOCITOFONIA;
}

void VideoEntryPhone::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int page_id = getTextChild(page_node, "id").toInt();
		Page *p = 0;

		switch (page_id)
		{
		case INTERCOM:
			p = new Intercom(page_node);
			break;
		case VIDEO_CONTROL:
			p = new VideoControl(page_node);
			break;
		case CALL_EXCLUSION:
			call_exclusion = new CallExclusionPage(page_node);
			p = call_exclusion;
			break;
		default:
			qFatal("Unhandled page id in VideoEntryPhone::loadItems");
		};

		if (p)
		{
			QString icon = bt_global::skin->getImage("link_icon");
			QString descr = getTextChild(item, "descr");
			addPage(p, descr, icon);
		}

	}
}


CallExclusionPage::CallExclusionPage(const QDomNode &config_node)
{
	buildPage();
	SkinContext context(getTextChild(config_node, "cid").toInt());
	b = new CallExclusion;
	page_content->appendBanner(b);
}

bool CallExclusionPage::getStatus()
{
	return b->getStatus();
}




VideoControl::VideoControl(const QDomNode &config_node)
{
	// we must have only one entryphone device since we need to remember some state
	dev = bt_global::add_device_to_cache(new EntryphoneDevice(bt_global::config[PI_ADDRESS]));

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(QString)), SLOT(cameraOn(QString)));

	buildPage(new IconContent, new NavigationBar);
	call_page = new VCTCallPage(dev);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		// TODO: aggiungere un check sull'id degli item relativi ai posti esterni!
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"), 0, 0);
		connect(btn, SIGNAL(clicked()), mapper, SLOT(map()));

		QString where = getTextChild(item, "dev") + getTextChild(item, "where");
		mapper->setMapping(btn, where);
	}

	connect(call_page, SIGNAL(Closed()), call_page, SLOT(showPreviousPage()));

}

void VideoControl::cameraOn(QString where)
{
	dev->cameraOn(where);
}


IntercomCallPage::IntercomCallPage(EntryphoneDevice *d)
{
	dev = d;
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	SkinContext ctx(666);

	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(10, 0, 0, 15);
	layout->setSpacing(5);
	layout->setColumnStretch(1, 1);

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), dev, SLOT(endCall()));
	connect(back, SIGNAL(clicked()), SLOT(handleClose()));
	layout->addWidget(back, 0, 0, 1, 1, Qt::AlignBottom);

	QGridLayout *buttons_layout = new QGridLayout;
	buttons_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), 0, 0, 1, 1);
	buttons_layout->setContentsMargins(0, 0, 0, 0);
	buttons_layout->setSpacing(15);

	QLabel *call_image = new QLabel;
	call_image->setPixmap(bt_global::skin->getImage("connection"));
	buttons_layout->addWidget(call_image, 1, 0, 1, 2);

	QString call_icon = bt_global::skin->getImage("call");
	call_accept = new BtButton;
	call_accept->setOnOff();
	call_accept->setImage(getBostikName(call_icon, "off"));
	call_accept->setPressedImage(getBostikName(call_icon, "on"));
	connect(call_accept, SIGNAL(clicked()), SLOT(toggleCall()));
	buttons_layout->addWidget(call_accept, 2, 0);

	QString mute_icon = bt_global::skin->getImage("mute");
	mute_button = new EnablingButton;
	mute_button->setImage(getBostikName(mute_icon, "off"));
	mute_button->setDisabledImage(getBostikName(getBostikName(mute_icon, "off"), "dis"));
	mute_button->setPressedImage(getBostikName(mute_icon, "on"));
	mute_button->setStatus(EnablingButton::DISABLED);
	connect(mute_button, SIGNAL(clicked()), SLOT(toggleMute()));
	buttons_layout->addWidget(mute_button, 2, 1);

	volume = new ItemTuning("", bt_global::skin->getImage("volume"));
	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
	buttons_layout->addWidget(volume, 3, 0, 1, 2, Qt::AlignHCenter);

	layout->addLayout(buttons_layout, 0, 1, Qt::AlignHCenter);
	prev_page = 0;
}

void IntercomCallPage::showPage()
{
	bt_global::display.forceOperativeMode(true);
	call_accept->setStatus(true);
	mute_button->setStatus(EnablingButton::DISABLED);
	Page::showPage();
}

void IntercomCallPage::showPageIncomingCall()
{
	bt_global::display.forceOperativeMode(true);
	call_accept->setStatus(false);
	mute_button->setStatus(EnablingButton::OFF);
	Page::showPage();
}


void IntercomCallPage::handleClose()
{
	bt_global::display.forceOperativeMode(false);
	if (prev_page)
	{
		prev_page->showPage();
		prev_page = 0;
	}
	else
		emit Closed();
}

void IntercomCallPage::toggleCall()
{
	bool connected = call_accept->getStatus();
	call_accept->setStatus(!connected);

	if (connected)
	{
		mute_button->setStatus(EnablingButton::DISABLED);
		dev->endCall();
		handleClose();
	}
	else
	{
		dev->answerCall();
		mute_button->setStatus(EnablingButton::OFF);
	}
}

void IntercomCallPage::toggleMute()
{
	EnablingButton::Status st = mute_button->getStatus();
	setVolume(VOLUME_MIC, st == EnablingButton::ON ? 0 : 1);

	if (st == EnablingButton::ON)
		mute_button->setStatus(EnablingButton::OFF);
	else
		mute_button->setStatus(EnablingButton::ON);
}

void IntercomCallPage::changeVolume(int value)
{
	setVolume(VOLUME_VIDEOCONTROL, value);
}

void IntercomCallPage::status_changed(const StatusList &sl)
{
	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::INTERCOM_CALL:
			prev_page = currentPage();
			showPageIncomingCall();
			break;
		case EntryphoneDevice::END_OF_CALL:
			handleClose();
			break;
		}
		++it;
	}
}


Intercom::Intercom(const QDomNode &config_node)
{
	EntryphoneDevice *dev = bt_global::add_device_to_cache(new EntryphoneDevice(bt_global::config[PI_ADDRESS]));

	mapper_int_intercom = new QSignalMapper(this);
	mapper_ext_intercom = new QSignalMapper(this);
	connect(mapper_int_intercom, SIGNAL(mapped(QString)), dev, SLOT(internalIntercomCall(QString)));
	connect(mapper_ext_intercom, SIGNAL(mapped(QString)), dev, SLOT(externalIntercomCall(QString)));

	IntercomCallPage *call_page = new IntercomCallPage(dev);
	connect(call_page, SIGNAL(Closed()), SLOT(showPage()));

	buildPage(new IconContent, new NavigationBar);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"), 0, 0);

		int id = getTextChild(item, "id").toInt();
		QString where = getTextChild(item, "dev") + getTextChild(item, "where");

		// TODO: aggiungere un check sull'id degli item relativi ai posti interni!
		if (1) // dovra' essere (id == INTERNAL_INTERCOM)
		{
			mapper_int_intercom->setMapping(btn, where);
			connect(btn, SIGNAL(clicked()), mapper_int_intercom, SLOT(map()));
		}
		else
		{
			mapper_ext_intercom->setMapping(btn, where);
			connect(btn, SIGNAL(clicked()), mapper_ext_intercom, SLOT(map()));
		}
		connect(btn, SIGNAL(clicked()), call_page, SLOT(showPage()));
	}
}

#endif
