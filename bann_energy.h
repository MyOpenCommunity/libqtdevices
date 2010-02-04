#ifndef BANN_ENERGY_H
#define BANN_ENERGY_H

#include "bann1_button.h"  // bannTextOnImage
#include "bann2_buttons.h" // Bann2Buttons
#include "energy_rates.h"  // EnergyRate
#include "device.h"        // StatusList

struct EnergyRate;


class bannEnergyInterface : public bannTextOnImage
{
Q_OBJECT
public:
	/**
	 * \param rate_id the id of the rate in the rate_info tag
	 * \param n_dec the number of decimals to show in the labels
	 * \param is_electricity true if we are measuring electricity
	 */
	bannEnergyInterface(QWidget *parent, int rate_id, bool is_ele);
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

#endif // BANN_ENERGY_H
