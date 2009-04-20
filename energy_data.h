#ifndef ENERGY_DATA_H
#define ENERGY_DATA_H

#include "sottomenu.h"

#include <QHash>

class bannTextOnImage;
class banner;
class QDomNode;


class EnergyData : public sottoMenu
{
Q_OBJECT
public:
	EnergyData(const QDomNode &config_node);

private:
	void loadTypes(const QDomNode &config_node);
};


/**
 * The page to set the related cost and incentive of an energy typology.
 */
class EnergyCost : public PageLayout
{
Q_OBJECT
public:
	EnergyCost(const QDomNode &config_node, int serial);

private slots:
	void closePage();
	void saveCostAndProd();
	void decreaseCost();
	void increaseCost();
	void decreaseProd();
	void increaseProd();

private:
	int serial_number;
	float delta;
	// The rates for consumption and production. The temp_ attribute are used
	// to modify the real values (the attributes without temp_) only when the user
	// click on the "ok" button.
	float cons_rate, prod_rate, temp_cons_rate, temp_prod_rate;
	banner *banner_cost, *banner_prod;
	// The number of decimal to show in cunsumption and production.
	int n_decimal;
	banner *addBanner(const QDomNode &config_node, QString desc, float& rate);
	void showValue(banner *b, float value);
};


class EnergyInterface : public sottoMenu
{
Q_OBJECT
public:
	EnergyInterface(const QDomNode &config_node);
	virtual void showPage();

public slots:
	void status_changed(const StatusList &status_list);

private:
	QString findAddress(const StatusList &sl);
	void loadItems(const QDomNode &config_node);
	Page *next_page;
	QHash<QString, bannTextOnImage*> banner_hash;
	int conversion_factor;
	QString measure;
};

#endif // ENERGY_DATA_H
