#ifndef AIR_CONDITIONING_H
#define AIR_CONDITIONING_H

#include "page.h"

class AirConditioningDevice;
class banner;
class QDomNode;



class AirConditioning : public BannerPage
{
Q_OBJECT
public:
	AirConditioning(const QDomNode &config_node);
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};


class SplitPage : public BannerPage
{
Q_OBJECT
public:
	SplitPage(const QDomNode &config_node, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
	QString off;

	void loadScenarios(const QDomNode &config_node);

private slots:
	void sendOff();
};


class AdvancedSplitPage : public BannerPage
{
Q_OBJECT
public:
	AdvancedSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};


class SplitSettings : public BannerPage
{
Q_OBJECT
public:
	SplitSettings();
};

#endif // AIR_CONDITIONING_H
