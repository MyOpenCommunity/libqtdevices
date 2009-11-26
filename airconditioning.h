#ifndef AIR_CONDITIONING_H
#define AIR_CONDITIONING_H

#include "page.h"

class QDomNode;
class banner;


class AirConditioning : public BannerPage
{
Q_OBJECT
public:
	AirConditioning(const QDomNode &config_node);
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};


#endif // AIR_CONDITIONING_H
