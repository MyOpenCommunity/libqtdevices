#ifndef LOAD_MANAGEMENT_H
#define LOAD_MANAGEMENT_H

#include "page.h"
#include "bttime.h"
#include "energy_rates.h" // EnergyRate

#include <QSignalMapper>
#include <QDate>

class QDomNode;
class banner;
class QLabel;
class DeactivationTime;
class Bann2Buttons;
class LoadsDevice;

class LoadManagement : public BannerPage
{
Q_OBJECT
public:
	LoadManagement(const QDomNode &config_node);

	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};


class ConfirmationPage : public Page
{
Q_OBJECT
public:
	ConfirmationPage(const QString &text);

signals:
	void accept();
	void cancel();
};


class LoadDataContent : public QWidget
{
Q_OBJECT
public:
	/**
	 * \param dec Number of decimals to display when in energy view
	 * \param _rate_id Rate id to be used for economic data conversions
	 */
	LoadDataContent(int dec, int _rate_id = -1);

	/**
	 * Set text on consumption label
	 */
	void setConsumptionValue(int new_value);

	/**
	 * Update time frame for the given period.
	 *
	 * \param period Extracted from frame
	 */
	void updatePeriodDate(int period, QDate date, BtTime time);

	/**
	 * Update consuption value for the given period.
	 *
	 * \param period Extracted from frame
	 */
	void updatePeriodValue(int period, int new_value);

public slots:
	void toggleCurrencyView();

private:
	void updateValues();
	QLabel *current_consumption;
	QSignalMapper mapper;
	Bann2Buttons *first_period, *second_period;
	int first_period_value, second_period_value, current_value;
	int rate_id, decimals;
	EnergyRate rate;
	bool is_currency;

private slots:
	void rateChanged(int id);

signals:
	void resetActuator(int);
};

class LoadDataPage : public Page
{
Q_OBJECT
public:
	// TODO: add LoadManagement device
	LoadDataPage(const QDomNode &config_node, LoadsDevice *d);

private:
	LoadDataContent *content;
	int reset_number;
	LoadsDevice *dev;

private slots:
	void status_changed(const StatusList &sl);
	void resetRequested(int which);
	void reset();
};



class DeactivationTimePage : public Page
{
Q_OBJECT
public:
	DeactivationTimePage(const QDomNode &config_node);

private slots:
	void sendDeactivateDevice();

private:
	DeactivationTime *content;
};

#endif // LOAD_MANAGEMENT_H
