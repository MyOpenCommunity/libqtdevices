#include "airconditioning.h"
#include "skinmanager.h"
#include "main.h"
#include "bann2_buttons.h"
#include "xml_functions.h"
#include "bannercontent.h"

#include <QDomNode>
#include <QString>


AirConditioning::AirConditioning(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

banner *AirConditioning::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	SkinContext context(getTextChild(item_node, "cid").toInt());
	QString descr = getTextChild(item_node, "descr");
	QString img_off = bt_global::skin->getImage("off");
	QString img_air_single = bt_global::skin->getImage("air_single");
	QString img_air_gen = bt_global::skin->getImage("air_general");
	QString img_forward = bt_global::skin->getImage("forward");

	banner *b = 0;
	switch (id)
	{
	case AIR_SPLIT:
	{
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_single, img_forward, descr);
		b = bann;
//		bann->connectRightButton(en_interf);
		break;
	}
	case AIR_GENERAL:
	{
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_gen, img_forward, descr);
		b = bann;
		break;
	}
	case AIR_SPLIT_ADV:
	{
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_single, img_forward, descr);
		b = bann;
		break;
	}
	case AIR_GENERAL_ADV:
	{
		BannOnOffNew *bann = new BannOnOffNew(0);
		bann->initBanner(img_off, img_air_gen, img_forward, descr);
		b = bann;
		break;
	}
	}

	if (b)
		b->setId(id);

	return b;
}

void AirConditioning::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		// TODO: il generale va messo nel top_widget se presente?
		if (banner *b = getBanner(item))
			page_content->appendBanner(b);
		else
		{
			int id = getTextChild(item, "id").toInt();
			qFatal("Type of item %d not handled on aircondition page!", id);
		}
	}
}

