#ifndef AIR_CONDITIONING_H
#define AIR_CONDITIONING_H

#include "page.h"
#include "airconditioning_device.h"

class AirConditioningInterface;
class AirConditioningDevice;
class banner;
class QDomNode;
class SplitSwing;
class SplitMode;
class SplitTemperature;
class SplitSpeed;
class NonControlledProbeDevice;

typedef AdvancedAirConditioningDevice::AirConditionerStatus AirConditionerStatus;

namespace AirConditioningPrivate
{
	// A global object that contains the devices for single splits (required
	// because the general off send the off for each single split device).
	class DeviceContainer : public QObject
	{
	Q_OBJECT
	public:
		void append(AirConditioningInterface *d);

	public slots:
		void sendGeneralOff();

	private:
		QList<AirConditioningInterface*> devices_list;
	};
}


// The main page of the Air Conditioning
class AirConditioning : public BannerPage
{
Q_OBJECT
public:
	AirConditioning(const QDomNode &config_node);
	static banner *getBanner(const QDomNode &item_node);
	virtual void inizializza();

private:
	static NonControlledProbeDevice *createProbeDevice(const QDomNode &item_node);
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

private slots:
	void setDeviceOff();
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

protected:
	virtual void showEvent(QShowEvent *);

private slots:
	void handleClose();
	void acceptChanges();

private:
	void readBannerValues();
	void readModeConfig(const QDomNode &mode_node, const QDomNode &values);
	void readTempConfig(const QDomNode &temp_node, const QDomNode &values);
	void readSwingConfig(const QDomNode &swing_node, const QDomNode &values);
	void readSpeedConfig(const QDomNode &speed_node, const QDomNode &values);
	void sendUpdatedValues();
	SplitSwing *swing;
	SplitTemperature *temperature;
	SplitMode *mode;
	SplitSpeed *speed;
	int current_mode, current_temp, current_fan_speed, current_swing;

signals:
	void splitSettingsChanged(const AirConditionerStatus &);
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


class AdvancedGeneralSplitPage : public BannerPage
{
Q_OBJECT
public:
	AdvancedGeneralSplitPage(const QDomNode &config_node);

private:
	void loadScenarios(const QDomNode &config_node);
};

#endif // AIR_CONDITIONING_H
