#ifndef BANN_LIGHTING_H
#define BANN_LIGHTING_H

#include "bannregolaz.h" // BannLevel
#include "bann2_buttons.h" // BannOnOffNew, BannOnOff2Labels
#include "bann1_button.h" // BannOn2Labels
#include "bttime.h" // BtTime
#include "device.h" // StatusList

#include <QList>
#include <QTimer>


class device;
class device_status;
class openwebnet;
class LightingDevice;
class QDomNode;
class DimmerDevice;
class Dimmer100Device;


class LightGroup : public Bann2Buttons
{
Q_OBJECT
public:
	LightGroup(const QList<QString> &addresses, const QString &descr);
	// TODO: do we need a inizializza() method? The original class didn't have it...

private slots:
	void lightOn();
	void lightOff();

private:
	// of course these pointers must not be deleted since objects are owned by
	// the cache
	QList<LightingDevice *> devices;
};


/*
 * A derived class of BannLevel which handles all dimmer state changes (both levels and states).
 */
class AdjustDimmer : public BannLevel
{
Q_OBJECT
protected:
	enum States
	{
		ON,
		OFF,
		BROKEN,
	};
	AdjustDimmer(QWidget *parent);
	void initBanner(const QString &left, const QString &center_left, const QString &center_right,
		const QString &right, const QString &broken, States init_state, int init_level,
		const QString &banner_text);
	void setLevel(int level);
	void setState(States new_state);

private:
	void setOnIcons();
	int current_level;
	States current_state;
	QString center_left, center_right, broken;
};

// TODO: to be renamed when dimmer is gone
class Dimmer : public AdjustDimmer
{
Q_OBJECT
public:
	Dimmer(const QString &descr, const QString &where);
	virtual void inizializza(bool forza = false);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();
	void status_changed(const StatusList &sl);

private:
	DimmerDevice *dev;
	int light_value;
};

class DimmerGroup : public BannLevel
{
Q_OBJECT
public:
	DimmerGroup(const QList<QString> &addresses, const QString &descr);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	QList<DimmerDevice *> devices;
};

// TODO: to be renamed when dimmer100 is gone
class Dimmer100 : public AdjustDimmer
{
Q_OBJECT
public:
	Dimmer100(const QString &descr, const QString &where, int _start_speed, int _stop_speed);
	virtual void inizializza(bool forza = false);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();
	void status_changed(const StatusList &sl);

private:
	int roundTo5(int value);
	Dimmer100Device *dev;
	int start_speed, stop_speed;
	int light_value;
};

class Dimmer100Group : public BannLevel
{
Q_OBJECT
public:
	Dimmer100Group(QWidget *parent, const QDomNode &config_node);

private slots:
	void lightOn();
	void lightOff();
	void increaseLevel();
	void decreaseLevel();

private:
	QList<Dimmer100Device *> devices;
	QList<int> start_speed, stop_speed;
};


class TempLight : public BannOnOff2Labels
{
Q_OBJECT
public:
	TempLight(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza);

protected:
	virtual void readTimes(const QDomNode &node);
	void updateTimeLabel();

	QList<BtTime> times;
	int time_index;
	LightingDevice *dev;

protected slots:
	virtual void activate();

private slots:
	void status_changed(const StatusList &sl);
	void cycleTime();
};

class TempLightVariable : public TempLight
{
Q_OBJECT
public:
	TempLightVariable(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza);

protected:
	virtual void readTimes(const QDomNode &node);

protected slots:
	virtual void activate();
};


class TempLightFixed : public BannOn2Labels
{
Q_OBJECT
public:
	TempLightFixed(QWidget *parent, const QDomNode &config_node);
	virtual void inizializza(bool forza);

private slots:
	void status_changed(const StatusList &sl);
	void setOn();
	void updateTimerLabel();

private:
	void stopTimer();
	int total_time;
	// we have received a valid variable timing update since last request
	bool valid_update;
	// retries when no response to a variable timing request has been received
	int update_retries;

	BtTime lighting_time;
	QTimer request_timer;
	LightingDevice *dev;
};

#endif
