#ifndef ENERGY_VIEW_H
#define ENERGY_VIEW_H

#include "page.h"

#include <QColor>
#include <QDate>
#include <QVector>
#include <QWidget>


class BtButton;
class banner; // TODO: this must be changed once the correct banner is ready
class EnergyDevice;
class QLabel;
class QStackedWidget;
class QSignalMapper;


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
	EnergyView(QString measure, QString energy_type, QString address);
	virtual void inizializza();

protected:
	void timerEvent(QTimerEvent *);

private:
	QWidget *buildBannerWidget();

	enum Widget
	{
		BANNER_WIDGET = 0,
		GRAPH_WIDGET = 1
	};

	enum GraphType
	{
		CUMULATIVE_DAY,
		CUMULATIVE_MONTH,
		CUMULATIVE_YEAR,
		DAILY_AVERAGE
	};

	// TODO: this must be changed once the correct banner is ready
	banner *current_banner, *daily_av_banner;
	banner *cumulative_day_banner, *cumulative_month_banner, *cumulative_year_banner;
	TimePeriodSelection *time_period;
	QStackedWidget *widget_container;
	Widget current_widget;
	EnergyDevice *dev;
	QString unit_measure;
	QSignalMapper *mapper;
	GraphType current_graph;

private slots:
	void toggleCurrency();
	void changeTimePeriod(int, QDate);
	void showGraph(int graph_type);
	void showBannerWidget();
	void backClick();
	void status_changed(const StatusList &status_list);
};
#endif // ENERGY_VIEW_H
