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
class EnergyCost : public BannerPage
{
Q_OBJECT
public:
	EnergyCost();
};


class EditEnergyCost : public BannerPage
{
Q_OBJECT
public:
	EditEnergyCost();

	void addRate(int rate_id);
};


class EnergyInterface : public BannerPage
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
	void loadItems(const QDomNode &config_node, NavigationBar *nav_bar);
	bool checkTypeForCurrency(const QString &type, const QDomNode &conf);
	void updateBanners();
	EnergyView *next_page;
	QList<EnergyView*> views;
	bool is_any_interface_enabled;
	static bool is_currency_view;
};

#endif // ENERGY_DATA_H
