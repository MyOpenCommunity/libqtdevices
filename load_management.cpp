#include "load_management.h"
#include "xml_functions.h"
#include "bannercontent.h"
#include "main.h"
#include "banner.h"

LoadManagement::LoadManagement(const QDomNode &config_node) :
	BannerPage(0)
{
	buildPage(getTextChild(config_node, "descr"));
}

void LoadManagement::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		banner *b = getBanner(item);
	}
}

banner *LoadManagement::getBanner(const QDomNode &item_node)
{
	int id = getTextChild(item_node, "id").toInt();
	QString where = getTextChild(item_node, "where");

	banner *b = 0;
	switch (id)
	{
	case LOAD_WITH_CU:
		break;
	case LOAD_WITHOUT_CU:
		break;
	}

	if (b)
		b->setId(id);
	return b;
}
