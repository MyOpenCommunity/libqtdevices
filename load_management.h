#ifndef LOAD_MANAGEMENT_H
#define LOAD_MANAGEMENT_H

#include "page.h"
#include "bttime.h"

#include <QSignalMapper>
#include <QDate>

class QDomNode;
class banner;
class QLabel;
class DeactivationTime;
class Bann2Buttons;

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
	LoadDataContent();
	/**
	 * Set text on consumption label
	 */
	void setConsumptionText(const QString &text);

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
	void updatePeriodValue(int period, const QString &text);

private:
	QLabel *current_consumption;
	QSignalMapper mapper;
	Bann2Buttons *first_period, *second_period;

signals:
	void resetActuator(int);
};

class LoadDataPage : public Page
{
Q_OBJECT
public:
	// TODO: add LoadManagement device
	LoadDataPage(const QDomNode &config_node);

private:
	LoadDataContent *content;
	int reset_number;

private slots:
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
