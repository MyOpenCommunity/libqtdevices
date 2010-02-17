#ifndef BANN_AUTOMATION_H
#define BANN_AUTOMATION_H

#include "bann1_button.h" // BannSinglePuls
#include "bann3_buttons.h" // Bann3Buttons
#include "bann2_buttons.h" // BannOpenClose
#include "device.h" // StatusList
#include "bttime.h" // BtTime

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

/**
 * Send open/close frame on button press and send stop on button release.
 *
 * Banner behaviour must change (both graphically and logically) depending on who was the origin of the action.
 * In case of update from bus, the icon must be STOP not pressed and a stop frame must be sent when the button
 * is clicked.
 * In case of command sent after the user pressed the touch screen, the icon must be STOP pressed and a stop
 * frame must be sent on button release.
 */
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
	GateEntryphoneActuator(const QString &descr, const QString &where);

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
	GateLightingActuator(const BtTime &t, const QString &descr, const QString &where);

private slots:
	void activate();

private:
	BtTime time;
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


class PPTStat : public BannerOld
{
Q_OBJECT
public:
	PPTStat(QWidget *parent, QString where);
	virtual void inizializza(bool forza=false);

private:
	PPTStatDevice *dev;
	QString img_open, img_close;
private slots:
	void status_changed(const StatusList &status_list);
};

#endif
