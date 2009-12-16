#ifndef AIR_CONDITIONING_H
#define AIR_CONDITIONING_H

#include "page.h"

class AirConditioningDevice;
class banner;
class QDomNode;



namespace AirConditioningPrivate
{
	// A global object that contains the devices for single splits (required
	// because the general off send the off for each single split device).
	class DeviceContainer : public QObject
	{
	Q_OBJECT
	public:
		void append(AirConditioningDevice *d);

	public slots:
		void sendGeneralOff();

	private:
		QList<AirConditioningDevice*> devices_list;
	};
}


// The main page of the Air Conditioning
class AirConditioning : public BannerPage
{
Q_OBJECT
public:
	AirConditioning(const QDomNode &config_node);
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};

/**
 * The page for a split (basic) which contains all the scenarios configured for
 * that split.
 */
class SplitPage : public BannerPage
{
Q_OBJECT
public:
	SplitPage(const QDomNode &config_node, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;

	void loadScenarios(const QDomNode &config_node);
};


/**
 * The page for a split (advanced) which contains all the scenarios configured
 * for that split.
 */
class AdvancedSplitPage : public BannerPage
{
Q_OBJECT
public:
	AdvancedSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};


/**
 * The page of the settings (also known as custom page) for an advanced scenario.
 */
class SplitSettings : public BannerPage
{
Q_OBJECT
public:
	SplitSettings(const QDomNode &values_node, const QDomNode &config_node);
};


/**
 * The page for a general split which contains all the scenarios for that.
 */
class GeneralSplitPage : public BannerPage
{
Q_OBJECT
public:
	GeneralSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};

#endif // AIR_CONDITIONING_H
