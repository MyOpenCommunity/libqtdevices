#ifndef BANN_AUTOMATION_H
#define BANN_AUTOMATION_H

#include "bann1_button.h" // BannSinglePuls
#include "bann3_buttons.h" // Bann3Buttons
#include "bann2_buttons.h" // BannOpenClose
#include "device.h" // StatusList

/// Forward Declarations
class device;
class device_status;
class PPTStatDevice;
class AutomationDevice;
class LightingDevice;
class QDomNode;


class InterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	InterblockedActuator(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza = false);

private slots:
	void sendGoUp();
	void sendGoDown();
	void sendStop();
	void status_changed(const StatusList &sl);

private:
	void connectButton(BtButton *btn, const char *slot);
	AutomationDevice *dev;
};


class SecureInterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	SecureInterblockedActuator(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza = false);

private slots:
	void sendOpen();
	void sendClose();
	void buttonReleased();
	void sendStop();
	void status_changed(const StatusList &sl);

private:
	void connectButtons();
	void changeButtonStatus(BtButton *btn);
	AutomationDevice *dev;
	bool is_any_button_pressed;
};

class GateEntryphoneActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateEntryphoneActuator(QWidget *parent, const QDomNode &config_node);

private slots:
	void activate();

private:
	QString where;
	device *dev;
};

class GateLightingActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateLightingActuator(QWidget *parent, const QDomNode &config_node);

private slots:
	void activate();

private:
	int time_h, time_m, time_s;
	LightingDevice *dev;
};


class InterblockedActuatorGroup : public Bann3Buttons
{
Q_OBJECT
public:
	InterblockedActuatorGroup(QWidget *parent, const QDomNode &config_node);

private slots:
	void sendOpen();
	void sendClose();
	void sendStop();

private:
	QList<AutomationDevice *> actuators;
};


class PPTStat : public banner
{
Q_OBJECT
public:
	PPTStat(QWidget *parent, QString where, int cid);
	virtual void inizializza(bool forza=false);

private:
	PPTStatDevice *dev;
	QString img_open, img_close;
private slots:
	void status_changed(const StatusList &status_list);
};

#endif
