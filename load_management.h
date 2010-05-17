#ifndef LOAD_MANAGEMENT_H
#define LOAD_MANAGEMENT_H

#include "page.h"
#include "energy_rates.h" // EnergyRate

#include <QSignalMapper>

class QDomNode;
class banner;
class QLabel;
class DeactivationTime;
class Bann2Buttons;
class LoadsDevice;
class BtTime;
class QDate;

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
	LoadDataContent(int currency_decimals, int _rate_id = -1);

	/**
	 * Set text on consumption label
	 */
	void setConsumptionValue(int new_value);

	/**
	 * Update time frame for the given period.
	 *
	 * \param period Extracted from frame
	 */
	void updatePeriodDate(int period, const QDate &date, const BtTime &time);

	/**
	 * Update consuption value for the given period.
	 *
	 * \param period Extracted from frame
	 */
	void updatePeriodValue(int period, qint64 new_value);

public slots:
	void toggleCurrencyView();

private:
	void updateValues();
	QLabel *current_consumption;
	QSignalMapper mapper;
	Bann2Buttons *first_period, *second_period;
	qint64 first_period_value, second_period_value, current_value;
	int rate_id, currency_decimals;
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

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);

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
	DeactivationTimePage(const QDomNode &config_node, LoadsDevice *d);

private slots:
	void sendDeactivateDevice();

private:
	DeactivationTime *content;
	LoadsDevice *dev;
};

#endif // LOAD_MANAGEMENT_H
