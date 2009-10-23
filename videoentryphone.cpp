#include "videoentryphone.h"
#include "bann_videoentryphone.h"
#include "xml_functions.h" // getTextChild, getChildren
#include "content_widget.h"
#include "main.h"


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
			qFatal("Type of device not handler by Video EntryPhone page!");
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
		content_widget->appendBanner(b);
	}
}
