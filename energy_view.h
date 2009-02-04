#ifndef ENERGY_VIEW_H
#define ENERGY_VIEW_H

#include "page.h"

#include <QColor>
#include <QDate>
#include <QVector>
#include <QWidget>

class QLabel;
class BtButton;
// TODO: this must be changed once the correct banner is ready
class banner;
class QStackedWidget;


class TimePeriodSelection : public QWidget
{
Q_OBJECT
public:
	enum TimePeriod
	{
		DAY,
		MONTH,
		YEAR,
	};
	TimePeriodSelection(QWidget *parent);
	QDate date();

private slots:
	void changeTimeScale();
	void periodForward();
	void periodBackward();

private:
	/**
	 * Changes the time period considered based on current status and the delta provided, ie.
	 * if status is DAY then delta days are added to selection_date.
	 * \param delta The number of days or months to be added to selection_date
	 */
	void changeTimePeriod(int delta);
	void setDate(QDate new_date);

	TimePeriod status;
	BtButton *back_period, *forw_period;
	QLabel *date_period_label;
	QDate selection_date;

signals:
	void timeChanged(int, QDate);
};


class EnergyView : public Page
{
Q_OBJECT
public:
	EnergyView(QString energy_type);

private:
	QWidget *buildBannerWidget();

	enum Widget
	{
		BANNER_WIDGET = 0,
		GRAPH_WIDGET = 1
	};

	// TODO: this must be changed once the correct banner is ready
	banner *cumulative_banner, *current_banner, *daily_av_banner;
	TimePeriodSelection *time_period;
	QStackedWidget *widget_container;
	Widget current_widget;

private slots:
	void toggleCurrency();
	void changeTimePeriod(int, QDate);
	void showGraphWidget();
	void showBannerWidget();
	void backClick();
};
#endif // ENERGY_VIEW_H
