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
class bannTextOnImage;
class EnergyDevice;
class EnergyTable;
class QLabel;
class QStackedWidget;
class QSignalMapper;
class bannFrecce;

typedef QCache<QString, GraphData> GraphCache;


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
	QString dateDisplayed();
	int status();

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
	QString formatDate(const QDate &date, TimePeriod period);

	TimePeriod _status;
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
	/**
	 * \param measure Measure unit for this energy type
	 * \param energy_type The energy type (will be the title of the page)
	 * \param address Address of the corresponding device
	 * \param mode Mode of the device
	 * \param currency_symbol The symbol of the currency. If it's a null string, then currency is not enabled
	 *    for this energy_type.
	 * \param is_production True if the data must be interpreted as production, false for consumption
	 */
	EnergyView(QString measure, QString energy_type, QString address, int mode, const QString &_currency_symbol,
		bool is_prod);
	~EnergyView();
	virtual void inizializza();
	void setProdFactor(float p);
	void setConsFactor(float c);

public slots:
	virtual void showPage();

protected:
	void timerEvent(QTimerEvent *);

private:
	QWidget *buildBannerWidget();
	GraphData *saveGraphInCache(const QVariant &v, EnergyDevice::GraphType t);
	QMap<int, float> convertGraphData(GraphData *gd);
	void setBannerPage(int status, const QDate &selection_date);
	QString dateToKey(const QDate &date, EnergyDevice::GraphType t);
	void updateBanners();
	void updateCurrentGraph();

	enum Widget
	{
		BANNER_WIDGET = 0,
		GRAPH_WIDGET = 1
	};

	bannFrecce *bannNavigazione;
	bannTextOnImage *current_banner, *daily_av_banner;
	bannTextOnImage *cumulative_day_banner, *cumulative_month_banner, *cumulative_year_banner;
	int current_value, daily_av_value;
	int cumulative_day_value, cumulative_month_value, cumulative_year_value;
	TimePeriodSelection *time_period;
	QStackedWidget *widget_container;
	EnergyTable *table;
	Widget current_widget;
	EnergyDevice *dev;
	QString unit_measure;
	QString currency_symbol;
	QSignalMapper *mapper;
	EnergyDevice::GraphType current_graph;
	QDate current_date;
	QHash<EnergyDevice::GraphType, GraphCache*> graph_data_cache;
	float cons_factor, prod_factor;
	bool is_electricity_view;
	bool is_production;

private slots:
	void toggleCurrency();
	void changeTimePeriod(int, QDate);
	void showGraph(int graph_type);
	void showBannerWidget();
	void backClick();
	void status_changed(const StatusList &status_list);
};


#endif // ENERGY_VIEW_H
