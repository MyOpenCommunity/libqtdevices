#ifndef ENERGY_DATA_H
#define ENERGY_DATA_H

#include "page.h" // BannerPage
#include "bann1_button.h" // bannPuls

#include <QHash>
#include <QTimer>

class banner;
class QDomNode;
class EnergyView;
class EnergyInterface;
struct EnergyRate;


class EnergyData : public BannerPage
{
Q_OBJECT
public:
	EnergyData(const QDomNode &config_node, bool edit_rates = true);

	virtual void showPage();

private slots:
	void systemTimeChanged();
	void updateInterfaces();
	void updateDayTimer();

private:
	void loadTypes(const QDomNode &config_node, bool edit_rates);

	QTimer day_timer;
	QList<EnergyInterface *> interfaces;
};


/**
 * The page to set the related cost and incentive of an energy typology.
 */
class EnergyCost : public BannerPage
{
Q_OBJECT
public:
	EnergyCost();

	virtual void showPage();

private:
	Page *next_page;
};


class EditEnergyCost : public BannerPage
{
Q_OBJECT
public:
	EditEnergyCost();

	void addRate(int rate_id);

private:
	// TODO CONFIG_BTOUCH
	int production_count, consumption_count;
};


class EnergyInterface : public BannerPage
{
Q_OBJECT
public:
	EnergyInterface(const QDomNode &config_node, bool edit_rates, bool parent_skipped);

	virtual void showPage();
	static void toggleCurrencyView();
	static bool isCurrencyView();
	void systemTimeChanged();

public slots:
	void toggleCurrency();

private:
	void loadItems(const QDomNode &config_node, NavigationBar *nav_bar);
	bool checkTypeForCurrency(const QString &type, const QDomNode &conf);
	void updateBanners();
	EnergyView *next_page;
	QList<EnergyView*> views;
	bool is_any_interface_enabled;
	static bool is_currency_view;
};

#endif // ENERGY_DATA_H
