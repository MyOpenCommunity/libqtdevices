#include "videoentryphone.h"
#include "bann_videoentryphone.h"
#include "xml_functions.h" // getTextChild, getChildren

#include <QDebug>

#include <assert.h>


VideoEntryPhone::VideoEntryPhone(QDomNode config_node)
{
	loadDevices(config_node);
}

void VideoEntryPhone::loadDevices(QDomNode config_node)
{
	QString unknown = getTextChild(config_node, "unknown");

	QDomNode device;
	foreach (device, getChildren(config_node, "device"))
	{
		int id = getTextChild(device, "id").toInt();
		if (id != POSTO_ESTERNO)
			assert(!"Type of device not handler by Video EntryPhone page!");
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
		appendBanner(b);
	}
}
