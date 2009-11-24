#include "bannerfactory.h"
#include "banner.h"
#include "lighting.h"
#include "automation.h"

banner *getBanner(const QDomNode &item_node)
{
	if (banner *b = Automation::getBanner(item_node))
		return b;
	if (banner *b = Lighting::getBanner(item_node))
		return b;

	return 0;
}
