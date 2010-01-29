#include "bannerfactory.h"
#include "banner.h"
#include "lighting.h"
#include "automation.h"
#include "settings.h"
#include "scenario.h"
#include "iconsettings.h"


banner *getBanner(const QDomNode &item_node)
{
	typedef banner* (*getBannerFunc)(const QDomNode &);
	QList<getBannerFunc> sections;
	sections.append(&Automation::getBanner);
	sections.append(&Lighting::getBanner);
	sections.append(&Scenario::getBanner);
	sections.append(&Settings::getBanner);
#ifdef LAYOUT_TOUCHX
	sections.append(&IconSettings::getBanner);
#endif

	for (int i = 0; i < sections.size(); ++i)
		if (banner *b = sections[i](item_node))
			return b;

	return 0;
}
