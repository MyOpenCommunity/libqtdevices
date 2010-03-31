#ifndef ENERGY_RATES_H
#define ENERGY_RATES_H

#include <QObject>
#include <QMap>

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
class EnergyRates : public QObject
{
Q_OBJECT
public:
	enum
	{
		INVALID_RATE = -1,
	};

	EnergyRates();

	// returns true if any rates are defined
	bool hasRates();

	// can be called multiple times, will load the rates only
	// the first time
	void loadRates();

	// returns the rate, fails if the rate is not present; if the id is INVALID_RATE,
	// returns an invalid rate
	EnergyRate getRate(int rate_id) const;

	// returns the list of all the rate ids
	QList<int> allRateId() const;

	// updates the given rate and sends rateChanged() signal
	void setRate(const EnergyRate &new_rate);

	// the global object containing energy rates
	static EnergyRates energy_rates;

signals:
	void rateChanged(int rate_id);

private:
	// TODO energy use a list if the rate ids are contiguous
	QMap<int, EnergyRate> rates;
};


#endif // ENERGY_RATES_H
