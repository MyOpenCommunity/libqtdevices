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


#include "energy_view.h"
#include "energy_graph.h"
#include "banner.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName, DateConversion::formatDateConfig
#include "fontmanager.h" // bt_global::font
#include "devices_cache.h" // bt_global::devices_cache
#include "skinmanager.h" // bt_global::skin
#include "transitionwidget.h"
#include "energy_data.h" // EnergyInterface
#include "energy_rates.h"
#include "bann_energy.h"
#include "bann2_buttons.h" // Bann2Buttons

#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QTimerEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSignalMapper>


#define POLLING_CURRENT 5 // time to refresh data visualized in the current banner (in sec.)
#define POLLING_CUMULATIVE_DAY 60 * 60 // time to refresh data visualized in the comulative day banner (in sec.)

namespace
{
// The language used for the floating point number
QLocale loc(QLocale::Italian);
}


namespace
{
	QHash<QString, QPixmap> cache;

	BtButton *getTrimmedButton(QWidget *parent, QString icon)
	{
	#define SM_BTN_WIDTH 60
	#define SM_BTN_HEIGHT 40
		BtButton *btn = new BtButton(parent);
		QPixmap tmp;
		if (cache.contains(icon))
			tmp = cache[icon];
		else
		{
			tmp = (*bt_global::icons_cache.getIcon(icon)).copy(0, 10, SM_BTN_WIDTH, SM_BTN_HEIGHT);
			cache[icon] = tmp;
		}
		btn->setPixmap(tmp);
		QString picon = getPressName(icon);
		if (cache.contains(picon))
			tmp = cache[picon];
		else
		{
			tmp = (*bt_global::icons_cache.getIcon(picon)).copy(0, 10, SM_BTN_WIDTH, SM_BTN_HEIGHT);
			cache[picon] = tmp;
		}
		btn->setPressedPixmap(tmp);
		return btn;
	}

	QLabel *getLabel(QWidget *parent, QString text, FontManager::Type t)
	{
		QLabel *label = new QLabel(parent);
		label->setFont(bt_global::font->get(t));
		label->setText(text);
		return label;
	}

	QWidget *createWidgetWithGridLayout()
	{
		QWidget *w = new QWidget;
		QGridLayout *main_layout = new QGridLayout(w);
		main_layout->setContentsMargins(0, 0, 0, 0);
		main_layout->setSpacing(0);
		main_layout->setColumnStretch(0, 1);
		main_layout->setColumnStretch(2, 1);
		return w;
	}

	void addWidgetToLayout(QWidget *parent, QWidget *child)
	{
		QGridLayout *l = static_cast<QGridLayout*>(parent->layout());
		l->addWidget(child, l->rowCount(), 1, 1, 1, Qt::AlignTop | Qt::AlignLeft);
//		l->addWidget(child, 1, Qt::AlignTop);
	}

	enum EnergyViewPage
	{
		DAILY_PAGE = 0,
		MONTHLY_PAGE,
		YEARLY_PAGE
	};
}


EnergyViewNavigation::EnergyViewNavigation()
{

#ifdef LAYOUT_BTOUCH
	BtButton *back_button = createButton(bt_global::skin->getImage("back"), SIGNAL(backClick()), 0);

	table_button = createButton(bt_global::skin->getImage("table"), SIGNAL(showTable()), 2);
	currency_button = createButton(bt_global::skin->getImage("currency"), SIGNAL(toggleCurrency()), 3);
#else
	BtButton *back_button = createButton(bt_global::skin->getImage("back"), SIGNAL(backClick()), 3);
#endif
}

void EnergyViewNavigation::showTableButton(bool show)
{
#ifdef LAYOUT_BTOUCH
	table_button->setVisible(show);
#else
	Q_ASSERT_X(false, "EnergyViewNavigation::showTableButton", "You can't call this on Touch X!");
#endif
}

void EnergyViewNavigation::showCurrencyButton(bool show)
{
#ifdef LAYOUT_BTOUCH
	currency_button->setVisible(show);
#else
	Q_ASSERT_X(false, "EnergyViewNavigation::showCurrencyButton", "You can't call this on Touch X!");
#endif
}


TimePeriodSelection::TimePeriodSelection(QWidget *parent) : QWidget(parent)
{
	_status = DAY;
	selection_date = QDate::currentDate();
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);

#ifdef LAYOUT_BTOUCH
	main_layout->setSpacing(0);
	back_period = getTrimmedButton(this, bt_global::skin->getImage("fast_backward"));
	forw_period = getTrimmedButton(this, bt_global::skin->getImage("fast_forward"));
	btn_cycle = getTrimmedButton(this, bt_global::skin->getImage("cycle"));
	date_period_label = getLabel(this, formatDate(selection_date, _status), FontManager::SMALLTEXT);
#else
	main_layout->setSpacing(5);
	back_period = new BtButton(bt_global::skin->getImage("fast_backward"), this);
	forw_period = new BtButton(bt_global::skin->getImage("fast_forward"), this);
	btn_cycle = new BtButton(bt_global::skin->getImage("cycle"), this);
	date_period_label = getLabel(this, formatDate(selection_date, _status), FontManager::SUBTITLE);
#endif

	back_period->setAutoRepeat(true);
	connect(back_period, SIGNAL(clicked()), SLOT(periodBackward()));
	main_layout->addWidget(back_period);

	main_layout->addWidget(date_period_label, 1, Qt::AlignCenter);

	forw_period->setAutoRepeat(true);
	connect(forw_period, SIGNAL(clicked()), SLOT(periodForward()));
	main_layout->addWidget(forw_period);

	connect(btn_cycle, SIGNAL(clicked()), SLOT(changeTimeScale()));
	main_layout->addWidget(btn_cycle);
}

QString TimePeriodSelection::formatDate(const QDate &date, TimePeriod period)
{
	switch (period)
	{
	case DAY:
		return DateConversions::formatDateConfig(date);
	case MONTH:
		// no need to modify the format to american
		return date.toString(QString("MM%1yy").arg(DateConversions::separator));

	case YEAR:
	default:
		return tr("Last 12 months");
	}
}

void TimePeriodSelection::hideCycleButton()
{
	btn_cycle->hide();
}

void TimePeriodSelection::showCycleButton()
{
	btn_cycle->show();
}

void TimePeriodSelection::displayDate()
{
	switch (_status)
	{
	case YEAR:
		back_period->hide();
		forw_period->hide();
		break;
	default:
		back_period->show();
		forw_period->show();
		break;
	}
	date_period_label->setText(formatDate(selection_date, _status));
}

void TimePeriodSelection::changeTimeScale()
{
	switch (_status)
	{
	case DAY:
		_status = MONTH;
		break;
	case MONTH:
		_status = YEAR;
		break;
	case YEAR:
		_status = DAY;
		break;
	}
	displayDate();
	emit timeChanged(_status, selection_date);
}

void TimePeriodSelection::setDate(QDate new_date)
{
	QDate current = QDate::currentDate();
	if (new_date > current)
		selection_date = current;
	else if (new_date < current.addYears(-1).addMonths(1))
		selection_date = current.addYears(-1).addMonths(1);
	else
		selection_date = new_date;
}

void TimePeriodSelection::forceDate(QDate new_date, TimePeriod period)
{
	setDate(new_date);
	_status = period;
	displayDate();
	emit timeChanged(_status, selection_date);
}

void TimePeriodSelection::changeTimePeriod(int delta)
{
	QDate previous_date = selection_date;
	switch (_status)
	{
	case DAY:
		setDate(selection_date.addDays(delta));
		date_period_label->setText(formatDate(selection_date, _status));
		break;
	case MONTH:
		setDate(selection_date.addMonths(delta));
		date_period_label->setText(formatDate(selection_date, _status));
		break;
	default:
		qWarning("changeTimePeriod called with status==YEAR");
		break;
	}
	if (selection_date != previous_date)
		emit timeChanged(_status, selection_date);
}

QDate TimePeriodSelection::date()
{
	return selection_date;
}

int TimePeriodSelection::status()
{
	return _status;
}

void TimePeriodSelection::periodForward()
{
	changeTimePeriod(1);
}

void TimePeriodSelection::periodBackward()
{
	changeTimePeriod(-1);
}

QString TimePeriodSelection::dateDisplayed()
{
	return date_period_label->text();
}


Bann2Buttons *getBanner(QString primary_text)
{
	Q_ASSERT_X(bt_global::skin->hasContext(), "getBanner", "Skin context not set!");
	Bann2Buttons *bann = new Bann2Buttons;
	bann->initBanner(QString(), bt_global::skin->getImage("bg_banner"), bt_global::skin->getImage("graph"),
			 primary_text);
	bann->setCentralText("---");

	return bann;
}


EnergyView::EnergyView(QString measure, QString energy_type, QString address, int mode, int rate_id, EnergyTable *_table,
		       EnergyGraph *_graph)
{
	// see comment about _table in EnergyInterface::loadItems
	rate = EnergyRates::energy_rates.getRate(rate_id);
	connect(&EnergyRates::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));

	Q_ASSERT_X(bt_global::skin->hasContext(), "EnergyView::EnergyView", "Skin context not set!");
	dev = bt_global::add_device_to_cache(new EnergyDevice(address, mode));
	is_electricity_view = (mode == 1);

	cumulative_day_value = cumulative_month_value = cumulative_year_value = 0;
	daily_av_value = current_value = 0;

	connect(dev, SIGNAL(valueReceived(const DeviceValues&)), SLOT(valueReceived(const DeviceValues&)));

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(int)), SLOT(showGraph(int)));

	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
#ifdef LAYOUT_BTOUCH
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
#else
	main_layout->setContentsMargins(30, 0, 30, 0);
	main_layout->setSpacing(5);
#endif

	main_layout->setAlignment(Qt::AlignTop);

	// title section
	main_layout->addWidget(getLabel(this, energy_type, FontManager::TEXT), 0, Qt::AlignCenter);

	time_period = new TimePeriodSelection;
	connect(time_period, SIGNAL(timeChanged(int, QDate)), SLOT(changeTimePeriod(int, QDate)));
	main_layout->addWidget(time_period);

	widget_container = new QStackedWidget;
	widget_container->addWidget(buildBannerWidget());

	nav_bar = new EnergyViewNavigation;
	table = _table;
	graph = _graph;

#ifdef LAYOUT_BTOUCH
	main_layout->addWidget(widget_container, 1);
	connect(nav_bar, SIGNAL(toggleCurrency()), SLOT(toggleCurrency()));
	connect(nav_bar, SIGNAL(showTable()), table, SLOT(showPage()));
#else
	table_button = new BtButton(bt_global::skin->getImage("table"), this);
	currency_button = new BtButton(bt_global::skin->getImage("currency"), this);

	QGridLayout *box_layout = new QGridLayout;
	box_layout->setSpacing(5);
	box_layout->setContentsMargins(0, 0, 0, 0);
	box_layout->addWidget(widget_container, 0, 0, 3, 1);
	box_layout->addWidget(table_button, 1, 1);
	box_layout->addWidget(currency_button, 2, 1);
	box_layout->setColumnMinimumWidth(1, table_button->width());

	main_layout->addLayout(box_layout, 1);
	connect(currency_button, SIGNAL(clicked()), SLOT(toggleCurrency()));
	connect(table_button, SIGNAL(clicked()), table, SLOT(showPage()));
#endif

	if (!rate.isValid())
		showCurrencyButton(false);

	connect(nav_bar, SIGNAL(backClick()), SLOT(backClick()));

	buildPage(content, nav_bar);

	// default period, sync with default period in TimePeriodSelection
	// this used to call changeTimePeriod(); doing the initialization here
	// avoids useless status requests to the device
	showGraph(EnergyDevice::CUMULATIVE_DAY, false);
	setBannerPage(TimePeriodSelection::DAY, QDate::currentDate());

	switch(mode)
	{
		case 1:
		case 5:
			unit_measure_med_inst = measure;
			unit_measure = measure + "h";
			break;
		case 2:
		case 3:
		case 4:
		default:
			unit_measure_med_inst = measure + "/h";
			unit_measure = measure;
			break;
	}
	cumulative_day_banner_timer_id = startTimer(POLLING_CUMULATIVE_DAY * 1000);

	// this must be after creating bannNavigazione, otherwise segfault
	showBannerWidget();
}

EnergyView::~EnergyView()
{
	QMutableHashIterator<EnergyDevice::GraphType, GraphCache*> it(graph_data_cache);
	while (it.hasNext())
	{
		it.next();
		delete it.value();
	}
}

void EnergyView::timerEvent(QTimerEvent *e)
{
	// Poll only if the selected day is today.
	if (current_banner->isVisible())
	{
		if (e->timerId() == cumulative_day_banner_timer_id)
			dev->requestCumulativeDay(QDate::currentDate());
		else
			Q_ASSERT_X(false, "EnergyView::timerEvent", qPrintable(QString::number(e->timerId())));
	}
}

void EnergyView::inizializza()
{
	// Ask for the data showed in the default period.
	dev->requestCurrent();
	dev->requestCumulativeDay(QDate::currentDate());
	dev->requestCumulativeDayGraph(QDate::currentDate());
}

QString EnergyView::dateToKey(const QDate &date, EnergyDevice::GraphType t)
{
	switch (t)
	{
	case EnergyDevice::CUMULATIVE_MONTH:
	case EnergyDevice::DAILY_AVERAGE:
		return date.toString("yyyyMM");
	case EnergyDevice::CUMULATIVE_YEAR:
		return "";
	case EnergyDevice::CUMULATIVE_DAY:
	default:
		return date.toString("yyyyMMdd");
	}
}

GraphData *EnergyView::saveGraphInCache(const QVariant &v, EnergyDevice::GraphType t)
{
	Q_ASSERT_X(v.canConvert<GraphData>(), "EnergyView::saveGraphInCache", "Cannot convert graph data");
	GraphData *d = new GraphData(v.value<GraphData>());
	if (!graph_data_cache.contains(t))
	{
		graph_data_cache[t] = new GraphCache;
		// keep at most a full month in cache, to avoid
		// excessive memory consumption
		graph_data_cache[t]->setMaxCost(31);
	}

	GraphCache *cache = graph_data_cache[t];
	QString key = dateToKey(d->date, t);
	cache->insert(key, d);
	return d;
}

void EnergyView::showPage()
{
	// disconnect all slots: can't disconnect a single slot on multiple objects
	disconnect(table, SIGNAL(Closed()), 0, 0);
	connect(table, SIGNAL(Closed()), SLOT(showPageFromTable()));
	time_period->forceDate(QDate::currentDate());
	showPageFromTable();
}

void EnergyView::showPageFromTable()
{
	// switch back to raw data visualization if currency is not supported
	if (EnergyInterface::isCurrencyView() && !rate.isValid())
		EnergyInterface::toggleCurrencyView();
	Page::showPage();
}

QMap<int, float> EnergyView::convertGraphData(GraphData *gd)
{
	// TODO: remove the theorical bottleneck using a QMutableMapIterator instead of
	// accessing to the values using the operator[]
	QMap<int, float> data;
	// copy map to float values
	QList<int> graph_keys = gd->graph.keys();
	for (int i = 0; i < graph_keys.size(); ++i)
		data[graph_keys[i]] = gd->graph[graph_keys[i]];

	// convert to raw data
	QList<int> keys = data.keys();
	for (int i = 0; i < keys.size(); ++i)
		data[keys[i]] = EnergyConversions::convertToRawData(static_cast<int>(data[keys[i]]),
			is_electricity_view ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);

	// convert to economic data
	if (EnergyInterface::isCurrencyView())
	{
		for (int i = 0; i < keys.size(); ++i)
			data[keys[i]] = EnergyConversions::convertToMoney(data[keys[i]], rate.rate);
	}

	return data;
}

bool EnergyView::isGraphOurs()
{
	return graph->parent() == widget_container;
}

void EnergyView::valueReceived(const DeviceValues &values_list)
{
	current_date = time_period->date();
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case EnergyDevice::DIM_CUMULATIVE_DAY:
		{
			EnergyValue val = it.value().value<EnergyValue>();
			if (current_date == val.first)
				cumulative_day_value = val.second;
		}
			break;
		case EnergyDevice::DIM_CUMULATIVE_MONTH:
		{
			EnergyValue val = it.value().value<EnergyValue>();
			if (current_date.year() == val.first.year() && current_date.month() == val.first.month())
				cumulative_month_value = val.second;
		}
			break;
		case EnergyDevice::DIM_CUMULATIVE_YEAR:
		{
			EnergyValue val = it.value().value<EnergyValue>();
			if (current_date.year() == val.first.year())
				cumulative_year_value = val.second;
		}
			break;
		case EnergyDevice::DIM_MONTLY_AVERAGE:
		{
			EnergyValue val = it.value().value<EnergyValue>();
			if (current_date.year() == val.first.year() && current_date.month() == val.first.month())
				daily_av_value = val.second;
		}
			break;
		case EnergyDevice::DIM_CURRENT:
			current_value = it.value().value<EnergyValue>().second;
			break;
		case EnergyDevice::DIM_DAILY_AVERAGE_GRAPH:
		{
			GraphData *d = saveGraphInCache(it.value(), EnergyDevice::DAILY_AVERAGE);
			// The setData function of the graph object is called to force the refreshing of the graph
			// even if the graph page is already open.
			const QDate &date = d->date;
			if (current_graph == EnergyDevice::DAILY_AVERAGE && date.year() == current_date.year() &&
				date.month() == current_date.month() && isGraphOurs())
			{
				QMap<int, float> g = convertGraphData(d);
				graph->setData(g);
				table->setData(g);
			}
			break;
		}
		case EnergyDevice::DIM_DAY_GRAPH:
		{
			GraphData *d = saveGraphInCache(it.value(), EnergyDevice::CUMULATIVE_DAY);
			if (current_graph == EnergyDevice::CUMULATIVE_DAY && d->date == current_date &&
			    isGraphOurs())
			{
				QMap<int, float> g = convertGraphData(d);
				graph->setData(g);
				table->setData(g);
			}
			break;
		}
		case EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH:
		{
			GraphData *d = saveGraphInCache(it.value(), EnergyDevice::CUMULATIVE_MONTH);
			if (current_graph == EnergyDevice::CUMULATIVE_MONTH && d->date.month() == current_date.month()
				&& d->date.year() == current_date.year() && isGraphOurs())
			{
				QMap<int, float> g = convertGraphData(d);
				graph->setData(g);
				table->setData(g);
			}
			break;
		}
		case EnergyDevice::DIM_CUMULATIVE_YEAR_GRAPH:
		{
			// TODO: see how to save the year graph...
			GraphData *d = saveGraphInCache(it.value(), EnergyDevice::CUMULATIVE_YEAR);
			if (current_graph == EnergyDevice::CUMULATIVE_YEAR && isGraphOurs())
			{
				QMap<int, float> g = convertGraphData(d);
				graph->setData(g);
				table->setData(g);
			}
			break;
		}
		}
		++it;
	}
	updateBanners();
}

void EnergyView::backClick()
{
	if (current_widget == BANNER_WIDGET)
		emit Closed();
	else
		showBannerWidget();
}

void EnergyView::updateCurrentGraph()
{
	if (!isGraphOurs())
		return;

	current_date = time_period->date();
	QString label = EnergyInterface::isCurrencyView() ? rate.currency_symbol : unit_measure;
	QMap<int, QString> graph_x_axis;

	switch (current_graph)
	{
	case EnergyDevice::DAILY_AVERAGE:
	case EnergyDevice::CUMULATIVE_DAY:
		graph->init(24, label + tr("/hours"), graph_x_axis);
		table->init(24, tr("Hour"), label, time_period->dateDisplayed());
		break;
	case EnergyDevice::CUMULATIVE_YEAR:
	{
		int curr_month = QDate::currentDate().month();
		for (int i = 1; i <= 12; ++i)
		{
			int month = (curr_month  + i) % 12;
			if (month == 0)
				month = 12;
			graph_x_axis[i] = QString::number(month);
		}

		graph->init(12, label + tr("/months"), graph_x_axis);
		table->init(12, tr("Month"), label, time_period->dateDisplayed(), curr_month % 12);
		break;
	}
	case EnergyDevice::CUMULATIVE_MONTH:
	default:
		graph->init(time_period->date().daysInMonth(), label + tr("/days"), graph_x_axis);
		table->init(time_period->date().daysInMonth(), tr("Day"), label, time_period->dateDisplayed());
		break;
	}
#ifdef TEST_ENERGY_GRAPH
	graph->generateRandomValues();
	table->setData(graph->graph_data);
	return;
#endif
	QString key = dateToKey(current_date, current_graph);
	if (graph_data_cache.contains(current_graph) && graph_data_cache[current_graph]->contains(key))
	{
		GraphData *d = graph_data_cache[current_graph]->object(key);
		QMap<int, float> g = convertGraphData(d);
		graph->setData(g);
		table->setData(g);
	}
}

void EnergyView::showGraph(int graph_type, bool request_update)
{
	if (request_update)
	{
		QDate selected_date = time_period->date();
		switch (graph_type)
		{
		case EnergyDevice::CUMULATIVE_DAY:
			dev->requestCumulativeDayGraph(selected_date);
			break;
		case EnergyDevice::CUMULATIVE_MONTH:
			dev->requestCumulativeMonthGraph(selected_date);
			break;
		case EnergyDevice::DAILY_AVERAGE:
			dev->requestDailyAverageGraph(selected_date);
			break;
		case EnergyDevice::CUMULATIVE_YEAR:
		default:
			dev->requestCumulativeYearGraph();
			break;
		}
	}

	current_widget = GRAPH_WIDGET;
	current_graph = static_cast<EnergyDevice::GraphType>(graph_type);
	if (!isGraphOurs())
	{
		qDebug() << "Reparenting the graph";
		widget_container->addWidget(graph);
	}

	updateCurrentGraph();

	prepareTransition();
	showTableButton(true);
	widget_container->setCurrentIndex(current_widget);
	if (current_graph == EnergyDevice::DAILY_AVERAGE)
		time_period->hideCycleButton();

	startTransition();
}

void EnergyView::showTableButton(bool show)
{
#ifdef LAYOUT_BTOUCH
	nav_bar->showTableButton(show);
#else
	table_button->setVisible(show);
#endif
}

void EnergyView::showCurrencyButton(bool show)
{
#ifdef LAYOUT_BTOUCH
	nav_bar->showCurrencyButton(show);
#else
	currency_button->setVisible(show);
#endif
}

void EnergyView::showBannerWidget()
{
	current_widget = BANNER_WIDGET;
	prepareTransition();
	showTableButton(false);
	time_period->showCycleButton();
	widget_container->setCurrentIndex(current_widget);
	startTransition();
}

QWidget *EnergyView::buildBannerWidget()
{
	QString cumulative_text = tr("Cumulative");

	// Daily page
	cumulative_day_banner = getBanner(cumulative_text);
	connect(cumulative_day_banner, SIGNAL(rightClicked()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_day_banner, EnergyDevice::CUMULATIVE_DAY);

	current_banner = new BannCurrentEnergy(tr("Current"), dev);


	QWidget *daily_widget = createWidgetWithGridLayout();
	addWidgetToLayout(daily_widget, cumulative_day_banner);
	addWidgetToLayout(daily_widget, current_banner);

	// Monthly page
	cumulative_month_banner = getBanner(cumulative_text);
	connect(cumulative_month_banner, SIGNAL(rightClicked()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_month_banner, EnergyDevice::CUMULATIVE_MONTH);

	daily_av_banner = getBanner(tr("Daily Average"));
	connect(daily_av_banner, SIGNAL(rightClicked()), mapper, SLOT(map()));
	mapper->setMapping(daily_av_banner, EnergyDevice::DAILY_AVERAGE);

	QWidget *monthly_widget = createWidgetWithGridLayout();
	addWidgetToLayout(monthly_widget, cumulative_month_banner);
	addWidgetToLayout(monthly_widget, daily_av_banner);

	// Yearly page
	cumulative_year_banner = getBanner(cumulative_text);
	connect(cumulative_year_banner, SIGNAL(rightClicked()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_year_banner, EnergyDevice::CUMULATIVE_YEAR);

	QWidget *yearly_widget = createWidgetWithGridLayout();
	addWidgetToLayout(yearly_widget, cumulative_year_banner);

	QStackedWidget *w = new QStackedWidget;
	w->insertWidget(DAILY_PAGE, daily_widget);
	w->insertWidget(MONTHLY_PAGE, monthly_widget);
	w->insertWidget(YEARLY_PAGE, yearly_widget);

	w->setCurrentIndex(DAILY_PAGE); // default page

	return w;
}

void EnergyView::changeTimePeriod(int status, QDate selection_date)
{
	int graph_type = EnergyDevice::CUMULATIVE_DAY;

	switch (status)
	{
	case TimePeriodSelection::DAY:
		graph_type = EnergyDevice::CUMULATIVE_DAY;
		dev->requestCumulativeDay(selection_date);
		dev->requestCumulativeDayGraph(selection_date);
		break;
	case TimePeriodSelection::MONTH:
		// we have to preserve the current visualized graph (can be daily average)
		if (current_graph == EnergyDevice::DAILY_AVERAGE)
		{
			graph_type = EnergyDevice::DAILY_AVERAGE;
			dev->requestDailyAverageGraph(selection_date);
		}
		else
		{
			graph_type = EnergyDevice::CUMULATIVE_MONTH;
			dev->requestCumulativeMonthGraph(selection_date);
		}
		dev->requestCumulativeMonth(selection_date);
		dev->requestMontlyAverage(selection_date);
		break;
	case TimePeriodSelection::YEAR:
		graph_type = EnergyDevice::CUMULATIVE_YEAR;
		dev->requestCumulativeYear();
		dev->requestCumulativeYearGraph();
		break;
	}
	if (widget_container->currentIndex() == GRAPH_WIDGET)
		showGraph(graph_type, false);

	setBannerPage(status, selection_date);
}

void EnergyView::setBannerPage(int status, const QDate &selection_date)
{
	QStackedWidget *w = static_cast<QStackedWidget*>(widget_container->widget(BANNER_WIDGET));

	switch (status)
	{
	case TimePeriodSelection::DAY:
		w->setCurrentIndex(DAILY_PAGE);
		current_banner->setVisible(QDate::currentDate() == selection_date);
		break;
	case TimePeriodSelection::MONTH:
		w->setCurrentIndex(MONTHLY_PAGE);
		break;
	case TimePeriodSelection::YEAR:
		w->setCurrentIndex(YEARLY_PAGE);
		break;
	}
}

void EnergyView::toggleCurrency()
{
	EnergyInterface::toggleCurrencyView();
	table->setNumDecimal(3);
	updateBanners();
	updateCurrentGraph();
}

void EnergyView::updateBanners()
{
	float day = EnergyConversions::convertToRawData(cumulative_day_value,
		is_electricity_view ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);
	float current = EnergyConversions::convertToRawData(current_value,
		is_electricity_view ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);
	float month = EnergyConversions::convertToRawData(cumulative_month_value,
		is_electricity_view ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);
	float year = EnergyConversions::convertToRawData(cumulative_year_value,
		is_electricity_view ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);
	float average = EnergyConversions::convertToRawData(daily_av_value,
		is_electricity_view ? EnergyConversions::ELECTRICITY : EnergyConversions::OTHER_ENERGY);
	QString str = unit_measure;
	QString str_med_inst = unit_measure_med_inst;

	// The number of decimals to show depends on the visualization mode
	int dec = 0, dec_current = 0;

	// display values > 1 kW as kilowatts, lower values as watts
	if (is_electricity_view)
	{
		dec = 3;

		if (current >= 1)
		{
			dec_current = 3;
			str_med_inst = "kW";
		}
		else
		{
			current = current * 1000;
			dec_current = 0;
			str_med_inst = "W";
		}
	}

	if (EnergyInterface::isCurrencyView())
	{
		day = EnergyConversions::convertToMoney(day, rate.rate);
		current = EnergyConversions::convertToMoney(current, rate.rate);
		month = EnergyConversions::convertToMoney(month, rate.rate);
		year = EnergyConversions::convertToMoney(year, rate.rate);
		average = EnergyConversions::convertToMoney(average, rate.rate);
		str = rate.currency_symbol;
		str_med_inst = rate.currency_symbol+"/h";
		dec = 3;
	}

	cumulative_day_banner->setCentralText(QString("%1 %2")
		.arg(loc.toString(day, 'f', dec)).arg(str));

	cumulative_month_banner->setCentralText(QString("%1 %2")
		.arg(loc.toString(month, 'f', dec)).arg(str));

	cumulative_year_banner->setCentralText(QString("%1 %2")
		.arg(loc.toString(year, 'f', dec)).arg(str));

	daily_av_banner->setCentralText(QString("%1 %2")
		.arg(loc.toString(average, 'f', dec)).arg(str));

	current_banner->setCentralText(QString("%1 %2")
		.arg(loc.toString(current, 'f', dec_current)).arg(str_med_inst));
}

void EnergyView::systemTimeChanged()
{
	changeTimePeriod(time_period->status(), time_period->date());
}

void EnergyView::rateChanged(int rate_id)
{
	if (rate.id != rate_id)
		return;

	rate = EnergyRates::energy_rates.getRate(rate_id);
	updateBanners();
}
