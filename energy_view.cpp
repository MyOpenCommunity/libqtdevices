#include "energy_view.h"
#include "energy_graph.h"
#include "banner.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName
#include "fontmanager.h" // bt_global::font
#include "bannfrecce.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "skinmanager.h" // bt_global::skin
#include "transitionwidget.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSignalMapper>


#define POLLING_CURRENT_DATA 5 // time to refresh data visualized in the current banner (in sec.)
#define ENERGY_GRAPH_DELAY 1000 // msec to wait before request a graph data

// To use QDate as a key in QHash
inline bool qHash(const QDate &date) { return qHash(date.toString()); }


namespace
{
	BtButton *getTrimmedButton(QWidget *parent, QString icon)
	{
	#define SM_BTN_WIDTH 60
	#define SM_BTN_HEIGHT 40
		BtButton *btn = new BtButton(parent);
		QPixmap tmp = (*bt_global::icons_cache.getIcon(icon)).copy(0, 10, SM_BTN_WIDTH, SM_BTN_HEIGHT);
		btn->setPixmap(tmp);
		tmp = (*bt_global::icons_cache.getIcon(getPressName(icon))).copy(0, 10, SM_BTN_WIDTH, SM_BTN_HEIGHT);
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

	QWidget *createWidgetWithVBoxLayout()
	{
		QWidget *w = new QWidget;
		QVBoxLayout *main_layout = new QVBoxLayout;
		main_layout->setContentsMargins(0, 0, 0, 0);
		main_layout->setSpacing(0);
		w->setLayout(main_layout);
		return w;
	}

	enum EnergyViewPage
	{
		DAILY_PAGE = 0,
		MONTHLY_PAGE,
		YEARLY_PAGE
	};
}


TimePeriodSelection::TimePeriodSelection(QWidget *parent) : QWidget(parent)
{
	status = DAY;
	selection_date = QDate::currentDate();
	QHBoxLayout *main_layout = new QHBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	back_period = getTrimmedButton(this, bt_global::skin->getImage("fast_backward"));
	back_period->setAutoRepeat(true);
	connect(back_period, SIGNAL(clicked()), SLOT(periodBackward()));
	main_layout->addWidget(back_period);

	date_period_label = getLabel(this, selection_date.toString("dd/MM/yy"), FontManager::SMALLTEXT);
	main_layout->addWidget(date_period_label, 1, Qt::AlignCenter);

	forw_period = getTrimmedButton(this, bt_global::skin->getImage("fast_forward"));
	forw_period->setAutoRepeat(true);
	connect(forw_period, SIGNAL(clicked()), SLOT(periodForward()));
	main_layout->addWidget(forw_period);

	btn_cycle = getTrimmedButton(this, bt_global::skin->getImage("cycle"));
	connect(btn_cycle, SIGNAL(clicked()), SLOT(changeTimeScale()));
	main_layout->addWidget(btn_cycle);
	setLayout(main_layout);
}

void TimePeriodSelection::hideCycleButton()
{
	btn_cycle->hide();
}

void TimePeriodSelection::showCycleButton()
{
	btn_cycle->show();
}

void TimePeriodSelection::changeTimeScale()
{
	switch (status)
	{
	case DAY:
		status = MONTH;
		back_period->show();
		forw_period->show();
		date_period_label->setText(selection_date.toString("MM/yy"));
		break;
	case MONTH:
		status = YEAR;
		back_period->hide();
		forw_period->hide();
		date_period_label->setText(tr("Last 12 months"));
		break;
	case YEAR:
		status = DAY;
		back_period->show();
		forw_period->show();
		date_period_label->setText(selection_date.toString("dd/MM/yy"));
		break;
	}
	emit timeChanged(status, selection_date);
}

void TimePeriodSelection::setDate(QDate new_date)
{
	QDate current = QDate::currentDate();
	if (new_date > current)
		selection_date = current;
	else if (new_date < current.addYears(-1))
		selection_date = current.addYears(-1);
	else
		selection_date = new_date;
}

void TimePeriodSelection::changeTimePeriod(int delta)
{
	QDate previous_date = selection_date;
	switch (status)
	{
	case DAY:
		setDate(selection_date.addDays(delta));
		date_period_label->setText(selection_date.toString("dd/MM/yy"));
		break;
	case MONTH:
		setDate(selection_date.addMonths(delta));
		date_period_label->setText(selection_date.toString("MM/yy"));
		break;
	default:
		qWarning("periodForward called with status==YEAR");
		break;
	}
	if (selection_date != previous_date)
		emit timeChanged(status, selection_date);
}

QDate TimePeriodSelection::date()
{
	return selection_date;
}

void TimePeriodSelection::periodForward()
{
	changeTimePeriod(1);
}

void TimePeriodSelection::periodBackward()
{
	changeTimePeriod(-1);
}


banner *getBanner(QWidget *parent, QString primary_text)
{
	Q_ASSERT_X(bt_global::skin->hasContext(), "getBanner", "Skin context not set!");
	banner *bann = new banner(parent);
#define BANN_TEXT2_X 60
#define BANN_TEXT2_Y 0
#define BANN_TEXT2_WIDHT 120
#define BANN_TEXT2_HEIGHT 60
	bann->addItem(banner::TEXT2, BANN_TEXT2_X, BANN_TEXT2_Y, BANN_TEXT2_WIDHT, BANN_TEXT2_HEIGHT);
	bann->addItem(banner::BUT1, MAX_WIDTH-BANN_TEXT2_X, BANN_TEXT2_Y,  BANN_TEXT2_X, BANN_TEXT2_X);
	bann->addItem(banner::TEXT, BANN_TEXT2_Y, BANN_TEXT2_X, MAX_WIDTH, MAX_HEIGHT/NUM_RIGHE-BANN_TEXT2_X);
	bann->setText(primary_text);
	bann->setSecondaryText("100 kWh");
	bann->SetIcons(banner::BUT1, bt_global::skin->getImage("graph"));
	bann->Draw();
	return bann;
}


EnergyView::EnergyView(QString measure, QString energy_type, QString address, int mode, bool currency_enabled)
{
	Q_ASSERT_X(bt_global::skin->hasContext(), "EnergyView::EnergyView", "Skin context not set!");
	dev = bt_global::add_device_to_cache(new EnergyDevice(address, mode));
	dev->installFrameCompressor(ENERGY_GRAPH_DELAY);
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(int)), SLOT(showGraph(int)));

	main_layout->setAlignment(Qt::AlignTop);

	// title section
	main_layout->addWidget(getLabel(this, energy_type, FontManager::TEXT), 0, Qt::AlignCenter);

	time_period = new TimePeriodSelection(this);
	connect(time_period, SIGNAL(timeChanged(int, QDate)), SLOT(changeTimePeriod(int, QDate)));
	main_layout->addWidget(time_period);

	widget_container = new QStackedWidget;
	widget_container->addWidget(buildBannerWidget());
	widget_container->addWidget(new EnergyGraph);

	showBannerWidget();
	main_layout->addWidget(widget_container, 1);

	if (currency_enabled)
	{
		bannFrecce *nav_bar = new bannFrecce(this, 10, bt_global::skin->getImage("currency"));
		connect(nav_bar, SIGNAL(backClick()), SLOT(backClick()));
		connect(nav_bar, SIGNAL(dxClick()), SLOT(toggleCurrency()));
		main_layout->addWidget(nav_bar);
	}
	else
		addBackButton();

	// default period, sync with default period in TimePeriodSelection
	changeTimePeriod(TimePeriodSelection::DAY, QDate::currentDate());
	unit_measure = measure;
	startTimer(POLLING_CURRENT_DATA * 1000);
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

void EnergyView::timerEvent(QTimerEvent *)
{
	if (current_banner->isVisible())
		dev->requestCurrent();
}

void EnergyView::inizializza()
{
	// Ask for the data showed in the default period.
	dev->requestCurrent();
	dev->requestCumulativeDay(QDate::currentDate());
	dev->requestCumulativeDayGraph(QDate::currentDate());
}

GraphData *EnergyView::saveGraphInCache(const QVariant &v, EnergyDevice::GraphType t)
{
	Q_ASSERT_X(v.canConvert<GraphData>(), "EnergyView::saveGraphInCache", "Cannot convert graph data");
	GraphData *d = new GraphData(v.value<GraphData>());
	if (!graph_data_cache.contains(t))
		graph_data_cache[t] = new GraphCache;

	GraphCache *cache = graph_data_cache[t];
	cache->insert(d->date, d);
	return d;
}

void EnergyView::status_changed(const StatusList &status_list)
{
	EnergyGraph *graph = static_cast<EnergyGraph*>(widget_container->widget(GRAPH_WIDGET));
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		switch (it.key())
		{
		case EnergyDevice::DIM_CUMULATIVE_DAY:
			cumulative_day_banner->setSecondaryText(QString("%1 kWh").arg(it.value().toInt()/1000));
			cumulative_day_banner->Draw();
			break;
		case EnergyDevice::DIM_CUMULATIVE_MONTH:
			cumulative_month_banner->setSecondaryText(QString("%1 kWh").arg(it.value().toInt()/1000));
			cumulative_month_banner->Draw();
			break;
		case EnergyDevice::DIM_CUMULATIVE_YEAR:
			cumulative_year_banner->setSecondaryText(QString("%1 kWh").arg(it.value().toInt()/1000));
			cumulative_year_banner->Draw();
			break;
		case EnergyDevice::DIM_CURRENT:
			current_banner->setSecondaryText(QString("%1·kW").arg(it.value().toInt()/1000.0, 0, 'f', 3));
			current_banner->Draw();
			break;
		case EnergyDevice::DIM_DAILY_AVERAGE_GRAPH:
		{
			GraphData *d = saveGraphInCache(it.value(), EnergyDevice::DAILY_AVERAGE);
			const QDate &date = d->date;
			if (current_graph == EnergyDevice::DAILY_AVERAGE && date.year() == current_date.year() &&
				date.month() == current_date.month())
				graph->setData(d->graph);
			break;
		}
		case EnergyDevice::DIM_DAY_GRAPH:
		{
			GraphData *d = saveGraphInCache(it.value(), EnergyDevice::CUMULATIVE_DAY);
			if (current_graph == EnergyDevice::CUMULATIVE_DAY && d->date == current_date)
				graph->setData(d->graph);
			break;
		}
		}
		++it;
	}
}

void EnergyView::backClick()
{
	if (current_widget == BANNER_WIDGET)
		emit Closed();
	else
		showBannerWidget();
}

void EnergyView::showGraph(int graph_type)
{
	EnergyGraph *graph = static_cast<EnergyGraph*>(widget_container->widget(GRAPH_WIDGET));

	current_widget = GRAPH_WIDGET;
	current_graph = static_cast<EnergyDevice::GraphType>(graph_type);
	current_date = time_period->date();
	switch (current_graph)
	{
	case EnergyDevice::DAILY_AVERAGE:
	case EnergyDevice::CUMULATIVE_DAY:
		graph->init(24, tr("Kwh/hours"));
		break;
	case EnergyDevice::CUMULATIVE_YEAR:
		graph->init(12, tr("Kwh/months"));
		break;
	case EnergyDevice::CUMULATIVE_MONTH:
	default:
		graph->init(time_period->date().daysInMonth(), tr("Kwh/days"));
		break;
	}

	if (graph_data_cache.contains(current_graph) && graph_data_cache[current_graph]->contains(current_date))
		graph->setData(graph_data_cache[current_graph]->object(current_date)->graph);

	initTransition();
	widget_container->setCurrentIndex(current_widget);
	if (current_graph == EnergyDevice::DAILY_AVERAGE)
		time_period->hideCycleButton();
	startTransition();
}

void EnergyView::showBannerWidget()
{
	current_widget = BANNER_WIDGET;
	initTransition();
	time_period->showCycleButton();
	widget_container->setCurrentIndex(current_widget);
	startTransition();
}

QWidget *EnergyView::buildBannerWidget()
{
	QString cumulative_text = tr("Cumulative");

	// Daily page
	cumulative_day_banner = getBanner(this, cumulative_text);
	connect(cumulative_day_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_day_banner, EnergyDevice::CUMULATIVE_DAY);

	current_banner = getBanner(this, tr("Current"));
	current_banner->nascondi(banner::BUT1);

	QWidget *daily_widget = createWidgetWithVBoxLayout();
	daily_widget->layout()->addWidget(cumulative_day_banner);
	daily_widget->layout()->addWidget(current_banner);

	// Monthly page
	cumulative_month_banner = getBanner(this, cumulative_text);
	connect(cumulative_month_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_month_banner, EnergyDevice::CUMULATIVE_MONTH);

	daily_av_banner = getBanner(this, tr("Daily Average"));
	connect(daily_av_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(daily_av_banner, EnergyDevice::DAILY_AVERAGE);

	QWidget *monthly_widget = createWidgetWithVBoxLayout();
	monthly_widget->layout()->addWidget(cumulative_month_banner);
	monthly_widget->layout()->addWidget(daily_av_banner);

	// Yearly page
	cumulative_year_banner = getBanner(this, cumulative_text);
	connect(cumulative_year_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_year_banner, EnergyDevice::CUMULATIVE_YEAR);

	QWidget *yearly_widget = createWidgetWithVBoxLayout();
	yearly_widget->layout()->addWidget(cumulative_year_banner);

	QStackedWidget *w = new QStackedWidget;
	w->insertWidget(DAILY_PAGE, daily_widget);
	w->insertWidget(MONTHLY_PAGE, monthly_widget);
	w->insertWidget(YEARLY_PAGE, yearly_widget);

	w->setCurrentIndex(DAILY_PAGE); // default page

	return w;
}

void EnergyView::changeTimePeriod(int status, QDate selection_date)
{
	if (widget_container->currentIndex() == BANNER_WIDGET)
	{
		QStackedWidget *w = static_cast<QStackedWidget*>(widget_container->widget(BANNER_WIDGET));
		switch (status)
		{
		case TimePeriodSelection::DAY:
			w->setCurrentIndex(DAILY_PAGE);
			dev->requestCumulativeDay(selection_date);
			dev->requestCumulativeDayGraph(selection_date);
			// The request for the current is automatically done by the timerEvent slot.
			current_banner->setVisible(QDate::currentDate() == selection_date);
			break;
		case TimePeriodSelection::MONTH:
			dev->requestCumulativeMonth(selection_date);
			dev->requestCumulativeMonthGraph(selection_date);
			w->setCurrentIndex(MONTHLY_PAGE);
			break;
		case TimePeriodSelection::YEAR:
			dev->requestCumulativeYear();
			// TODO: add the request for the graph year
			w->setCurrentIndex(YEARLY_PAGE);
			break;
		}
	}
	else // GRAPH_WIDGET
	{
		int graph_type;

		switch (status)
		{
		case TimePeriodSelection::DAY:
			graph_type = EnergyDevice::CUMULATIVE_DAY;
			dev->requestCumulativeDayGraph(selection_date);
			break;
		case TimePeriodSelection::YEAR:
			graph_type = EnergyDevice::CUMULATIVE_YEAR;
			dev->requestCumulativeMonthGraph(selection_date);
			break;
		case TimePeriodSelection::MONTH:
			graph_type = EnergyDevice::CUMULATIVE_MONTH;
			// TODO: add the request for the graph year
			break;
		}
		showGraph(graph_type);
	}
}

void EnergyView::toggleCurrency()
{
	qWarning("Not yet implemented");
}

