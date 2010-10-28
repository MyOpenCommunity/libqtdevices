/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef ENERGY_VIEW_H
#define ENERGY_VIEW_H

#include "page.h"
#include "energy_device.h" // GraphData
#include "energy_rates.h"  // EnergyRate
#include "navigation_bar.h" // AbstractNavigationBar

#include <QColor>
#include <QDate>
#include <QVector>
#include <QWidget>
#include <QCache>

class BtButton;
class Bann2Buttons;
class EnergyDevice;
class EnergyTable;
class EnergyGraph;
class QLabel;
class QStackedWidget;
class QSignalMapper;
class EnergyViewNavigation;
struct EnergyRate;

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
	TimePeriodSelection(QWidget *parent = 0);
	void hideCycleButton();
	void showCycleButton();
	QDate date();
	QString dateDisplayed();
	TimePeriod status();
	void forceDate(QDate new_date, TimePeriod period=DAY);

private slots:
	void changeTimeScale();
	void periodForward();
	void periodBackward();
	void emitTimeChanged();

private:
	/*
	 * Changes the time period considered based on current status and the delta provided, ie.
	 * if status is DAY then delta days are added to selection_date.
	 * \param delta The number of days or months to be added to selection_date
	 */
	void changeTimePeriod(int delta);
	QString formatDate(const QDate &date, TimePeriod period);
	void setDate(QDate new_date);
	void displayDate();

	TimePeriod _status;
	BtButton *back_period, *forw_period, *btn_cycle;
	QLabel *date_period_label;
	QDate selection_date;
	QTimer delayed_update;

signals:
	void timeChanged(int, QDate);
};


/*!
	\ingroup EnergyData
	\brief The custom navigation bar for the EnergyView, that shows buttons
	to toggle the currency and to show the EnergyTable data.
*/
class EnergyViewNavigation : public AbstractNavigationBar
{
Q_OBJECT
public:
	EnergyViewNavigation();

	void showTableButton(bool show);
	void showCurrencyButton(bool show);

signals:
	void backClick();
#ifdef LAYOUT_TS_3_5
	void toggleCurrency();
	void showTable();

private:
	BtButton *table_button;
	BtButton *currency_button;
#endif
};


/*!
	\ingroup EnergyData
	\brief Shows the consumption of an energy interface.

	The page displays the banners for the total consumptions of a day, a month
	or a year in the last 12 months.
	In addition, it allows to view more detailed data using a EnergyGraph graph
	or an EnergyTable table.
*/
class EnergyView : public Page
{
Q_OBJECT
public:
	/*
	 * \param measure Measure unit for this energy type
	 * \param energy_type The energy type (will be the title of the page)
	 * \param address Address of the corresponding device
	 * \param mode Mode of the device
	 * \param currency_symbol The symbol of the currency. If it's a null string, then currency is not enabled
	 *    for this energy_type.
	 * \param n_dec the number of decimals to show in the labels
	 * \param is_production True if the data must be interpreted as production, false for consumption
	 */
	EnergyView(QString measure, QString energy_type, QString address, int mode, int rate_id,
		   int currency_decimals, EnergyTable *_table, EnergyGraph *_graph);
	~EnergyView();
	virtual int sectionId() const;

	virtual void cleanUp();
	void systemTimeChanged();

public slots:
	virtual void showPage();
	virtual void showPageFromTable();

protected:
	void timerEvent(QTimerEvent *e);

private:
	static const unsigned int INVALID_VALUE = (unsigned int)-1;

	QWidget *buildBannerWidget();
	GraphData *saveGraphInCache(const QVariant &v, EnergyDevice::GraphType t);
	void updateGraphData(GraphData *gd);
	QMap<int, double> convertGraphData(GraphData *gd);
	void setBannerPage(int status, const QDate &selection_date);
	QString dateToKey(const QDate &date, EnergyDevice::GraphType t);
	void updateBanners();
	void updateBanner(Bann2Buttons *banner, qint64 value, int dec, QString symbol);
	void updateCurrentGraph();
	// returns true if we own the graph/table instances
	bool isGraphOurs();

	enum Widget
	{
		BANNER_WIDGET = 0,
		GRAPH_WIDGET = 1
	};

	EnergyViewNavigation *nav_bar;
	Bann2Buttons *current_banner, *daily_av_banner;
	Bann2Buttons *cumulative_day_banner, *cumulative_month_banner, *cumulative_year_banner;
	qint64 current_value, daily_av_value;
	qint64 cumulative_day_value, cumulative_month_value, cumulative_year_value;
	TimePeriodSelection *time_period;
	QStackedWidget *widget_container;
	EnergyTable *table;
	EnergyGraph *graph;
	Widget current_widget;
	EnergyDevice *dev;
	QString unit_measure;
	QString unit_measure_med_inst;
	QSignalMapper *mapper;
	EnergyDevice::GraphType current_graph;
	QDate current_date;
	QHash<EnergyDevice::GraphType, GraphCache*> graph_data_cache;
	bool is_electricity_view;
	// the id of the timers used to poll data
	int cumulative_day_banner_timer_id;
	EnergyRate rate;
	int currency_decimals;
	bool update_after_ssaver;
	bool is_current_page;

#ifdef LAYOUT_TS_10
	BtButton *table_button;
	BtButton *currency_button;
#endif

private slots:
	void screenSaverStarted(Page *prev_page);
	void screenSaverStopped();
	void toggleCurrency();
	void changeTimePeriod(int, QDate);
	void showGraph(int graph_type);
	void showBannerWidget();
	void backClick();
	void valueReceived(const DeviceValues &values_list);
	void rateChanged(int rate_id);
	void showTableButton(bool show);
	void showCurrencyButton(bool show);
	void handleClose();
};


#endif // ENERGY_VIEW_H
