#ifndef ENERGY_VIEW_H
#define ENERGY_VIEW_H

#include "page.h"
#include "energy_device.h" // GraphData

#include <QColor>
#include <QDate>
#include <QVector>
#include <QWidget>
#include <QCache>

class BtButton;
class banner; // TODO: this must be changed once the correct banner is ready
class EnergyDevice;
class QLabel;
class QStackedWidget;
class QSignalMapper;

typedef QCache<QDate, GraphData> GraphCache;


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
	void hideCycleButton();
	void showCycleButton();
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
	BtButton *back_period, *forw_period, *btn_cycle;
	QLabel *date_period_label;
	QDate selection_date;

signals:
	void timeChanged(int, QDate);
};


class EnergyView : public PageLayout
{
Q_OBJECT
public:
	EnergyView(QString measure, QString energy_type, QString address);
	~EnergyView();
	virtual void inizializza();

protected:
	void timerEvent(QTimerEvent *);

private:
	QWidget *buildBannerWidget();
	GraphData *saveGraphInCache(const QVariant &v, EnergyDevice::GraphType t);

	enum Widget
	{
		BANNER_WIDGET = 0,
		GRAPH_WIDGET = 1
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
	EnergyDevice::GraphType current_graph;
	QDate current_date;
	QHash<EnergyDevice::GraphType, GraphCache*> graph_data_cache;

private slots:
	void toggleCurrency();
	void changeTimePeriod(int, QDate);
	void showGraph(int graph_type);
	void showBannerWidget();
	void backClick();
	void status_changed(const StatusList &status_list);
};
#endif // ENERGY_VIEW_H
