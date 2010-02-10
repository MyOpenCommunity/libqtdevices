#ifndef BANN_ENERGY_H
#define BANN_ENERGY_H

#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // Bann3ButtonsLabel
#include "energy_rates.h"  // EnergyRate
#include "bttime.h" // BtTime
#include "device.h"        // StatusList

struct EnergyRate;
class EnergyDevice;


class BannEnergyInterface : public Bann2Buttons
{
Q_OBJECT
public:
	/**
	 * \param rate_id the id of the rate in the rate_info tag
	 * \param n_dec the number of decimals to show in the labels
	 * \param is_electricity true if we are measuring electricity
	 */
	BannEnergyInterface(int rate_id, bool is_ele, const QString &description);
	void setUnitMeasure(const QString &m);
	void updateText();

public slots:
	void status_changed(const StatusList &status_list);

private slots:
	void rateChanged(int rate_id);

private:
	int device_value;
	QString measure;
	bool is_electricity;
	EnergyRate rate;
};


// displays the current consumption value for the device; when shown,
// calls requestCurrentUpdateStart(), requestCurrentUpdateStop() when hidden
class BannCurrentEnergy : public Bann2Buttons
{
public:
	BannCurrentEnergy(const QString &text, EnergyDevice *dev);

protected:
	virtual void showEvent(QShowEvent *e);
	virtual void hideEvent(QHideEvent *e);

private:
	EnergyDevice *dev;
};


// allows modifying the tariff for the given rate_id
class BannEnergyCost : public Bann2Buttons
{
Q_OBJECT
public:
	BannEnergyCost(int rate_id, const QString &left, const QString &right,
		       const QString &descr);

	void saveRate();
	void resetRate();

private:
	void updateLabel();

private slots:
	void incRate();
	void decRate();

private:
	EnergyRate rate;
	float current_value;
};


// displays the current load for an actuator (normal, close to limit,
// above limit) using three icons
class BannLoadDiagnostic : public Bann2Buttons
{
Q_OBJECT
public:
	BannLoadDiagnostic(device *dev, const QString &description);

private:
	void setState(int state);

private slots:
	void status_changed(const StatusList &sl);

private:
	QString state_icon;
};


class BannLoadNoCU : public Bann3ButtonsLabel
{
Q_OBJECT
public:
	// TODO: Add device
	BannLoadNoCU(const QString &descr);
	void connectRightButton(Page *p);
};


class BannLoadWithCU : public Bann3ButtonsLabel
{
Q_OBJECT
public:
	enum Type
	{
		BASE_MODE,
		ADVANCED_MODE,
	};
	BannLoadWithCU(const QString &descr, Type t);
	void connectRightButton(Page *p);

signals:
	// this should be emitted only if the device is active
	void deactivateDevice();
};



class DeactivationTime : public Bann2Buttons
{
Q_OBJECT
public:
	DeactivationTime(const BtTime &start_time);
	// TODO: this interface supports user cancel. If it's not needed, just use a timeChanged() signal
	BtTime currentTime() const;
	void setCurrentTime(const BtTime &t);

private:
	void updateDisplay();
	BtTime current_time;

private slots:
	void plusClicked();
	void minusClicked();
};

#endif // BANN_ENERGY_H
