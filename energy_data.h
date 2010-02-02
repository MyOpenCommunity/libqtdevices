#ifndef ENERGY_DATA_H
#define ENERGY_DATA_H

#include "sottomenu.h"
#include "bann1_button.h" // bannPuls, bannTextOnImage

#include <QHash>
#include <QTimer>

class banner;
class QDomNode;
class EnergyView;
class EnergyInterface;
struct EnergyRate;


class EnergyData : public sottoMenu
{
Q_OBJECT
public:
	EnergyData(const QDomNode &config_node);

private slots:
	void systemTimeChanged();
	void updateInterfaces();
	void updateDayTimer();

private:
	void loadTypes(const QDomNode &config_node);

	QTimer day_timer;
	QList<EnergyInterface *> interfaces;
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
	unsigned int n_decimal;
	// The maximum number of integer to show in cunsumption and production.
	unsigned int n_integer;
	banner *addBanner(const QDomNode &config_node, QString desc, float& rate);
	void showValue(banner *b, float value);

signals:
	void prodValueChanged(float);
	void consValueChanged(float);
};


class EnergyInterface : public sottoMenu
{
Q_OBJECT
public:
	EnergyInterface(const QDomNode &config_node);
	virtual void showPage();
	static void toggleCurrencyView();
	static bool isCurrencyView();
	void systemTimeChanged();

public slots:
	void toggleCurrency();

private:
	void loadItems(const QDomNode &config_node);
	bool checkTypeForCurrency(const QString &type, const QDomNode &conf);
	void updateBanners();
	EnergyView *next_page;
	QList<EnergyView*> views;
	bool is_any_interface_enabled;
	static bool is_currency_view;
};


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

#endif // ENERGY_DATA_H
