#include "bannerfactory.h"
#include "banner.h"
#include "lighting.h"
#include "automation.h"
#include "scenario.h"
#ifdef LAYOUT_TOUCHX
#include "iconsettings.h"
#else
#include "settings.h"
#endif


banner *getBanner(const QDomNode &item_node)
{
	typedef banner* (*getBannerFunc)(const QDomNode &);
	QList<getBannerFunc> sections;
	sections.append(&Automation::getBanner);
	sections.append(&Lighting::getBanner);
	sections.append(&Scenario::getBanner);
#ifdef LAYOUT_TOUCHX
	sections.append(&IconSettings::getBanner);
#else
	sections.append(&Settings::getBanner);
#endif

	for (int i = 0; i < sections.size(); ++i)
		if (banner *b = sections[i](item_node))
			return b;

	return 0;
}
