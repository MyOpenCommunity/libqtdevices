#ifndef BANN_ENERGY_H
#define BANN_ENERGY_H

#include "bann1_button.h"

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

private:
	int device_value;
	QString measure;
	bool is_electricity;
	const EnergyRate &rate;
};

#endif // BANN_ENERGY_H
