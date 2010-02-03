#ifndef ENERGY_RATES_H
#define ENERGY_RATES_H

#include <QMap>
#include <QString>

class QDomNode;


// information on a single energy rate
struct EnergyRate
{
	bool isValid() const;

	// value of rate_id
	int id;

	// 1 -> Electricity, 2 -> Water, 3 -> Gas, 4 -> DHW, 5 -> Heating/Cooling
	int mode;
	bool is_production;
	float rate, delta;

	// digits to display
	int display_integers, display_decimals;

	// Electricity, Fotovoltaic, ...
	QString description;

	QString unit;
	QString currency_name;
	QString currency_symbol;
};


// object to read the rate list from disk
class EnergyRates
{
public:
	EnergyRates();

	// can be called multiple times, will load the rates only
	// the first time
	void loadRates();

	// returns the rate, fails if the rate is not present; if the id is -1,
	// returns an invalid rate
	EnergyRate getRate(int rate_id) const;

	// returns the list of all the rate ids
	QList<int> allRateId() const;

private:
	// TODO energy use a list if the rate ids are contiguous
	QMap<int, EnergyRate> rates;
};


namespace bt_global { extern EnergyRates energy_rates; }

#endif // ENERGY_RATES_H
