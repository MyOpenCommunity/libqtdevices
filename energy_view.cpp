#include "energy_view.h"
#include "energy_graph.h"
#include "banner.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName
#include "fontmanager.h" // bt_global::font
#include "bannfrecce.h"
#include "devices_cache.h" // bt_global::devices_cache

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSignalMapper>

#include <assert.h>

#define ICON_FWD IMG_PATH "btnforward.png"
#define ICON_BACK IMG_PATH "btnbackward.png"
#define ICON_AVANTI IMG_PATH "btnavanti.png"
// TODO: modificare con le icone corrette
#define ICON_GRAPH IMG_PATH "arrrg.png"
#define ICON_CURRENCY IMG_PATH "btncanc.png"

#define POLLING_CURRENT_DATA 5 // time to refresh data visualized in the current banner

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
		label->setFont(bt_global::font.get(t));
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

	back_period = getTrimmedButton(this, ICON_BACK);
	back_period->setAutoRepeat(true);
	connect(back_period, SIGNAL(clicked()), SLOT(periodBackward()));
	main_layout->addWidget(back_period);

	date_period_label = getLabel(this, selection_date.toString("dd/MM/yy"), FontManager::SMALLTEXT);
	main_layout->addWidget(date_period_label, 1, Qt::AlignCenter);

	forw_period = getTrimmedButton(this, ICON_FWD);
	forw_period->setAutoRepeat(true);
	connect(forw_period, SIGNAL(clicked()), SLOT(periodForward()));
	main_layout->addWidget(forw_period);

	BtButton *btn_cycle;
	btn_cycle = getTrimmedButton(this, ICON_AVANTI);
	connect(btn_cycle, SIGNAL(clicked()), SLOT(changeTimeScale()));
	main_layout->addWidget(btn_cycle);
	setLayout(main_layout);
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
	//TODO: do something sensible!! :)
	switch (status)
	{
	case DAY:
		setDate(selection_date.addDays(delta));
		date_period_label->setText(selection_date.toString("dd/MM/yy"));
		emit timeChanged(status, selection_date);
		break;
	case MONTH:
		setDate(selection_date.addMonths(delta));
		date_period_label->setText(selection_date.toString("MM/yy"));
		break;
	default:
		qWarning("periodForward called with status==YEAR");
		break;
	}
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
	bann->SetIcons(banner::BUT1, ICON_GRAPH);
	bann->Draw();
	return bann;
}


EnergyView::EnergyView(QString measure, QString energy_type, QString address)
{
	// TODO: gestire in un modo migliore!!
	EnergyDevice *d = new EnergyDevice(address);
	dev = static_cast<EnergyDevice*>(bt_global::devices_cache.add_device(d));
	connect(dev, SIGNAL(status_changed(const StatusList&)), SLOT(status_changed(const StatusList&)));
	if (dev != d)
		delete d;

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

	bannFrecce *nav_bar = new bannFrecce(this, 10, ICON_CURRENCY);
	connect(nav_bar, SIGNAL(backClick()), SLOT(backClick()));
	connect(nav_bar, SIGNAL(dxClick()), SLOT(toggleCurrency()));
	main_layout->addWidget(nav_bar);

	// default period, sync with default period in TimePeriodSelection
	changeTimePeriod(TimePeriodSelection::DAY, QDate::currentDate());
	unit_measure = measure;
	startTimer(POLLING_CURRENT_DATA * 1000);
}

EnergyView::~EnergyView()
{
	QMutableHashIterator<GraphType, GraphCache*> it(graph_data_cache);
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
	dev->requestCumulativeDay();
	dev->requestCumulativeDayGraph(QDate::currentDate());
}

void EnergyView::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		switch (it.key())
		{
		case EnergyDevice::DIM_CUMULATIVE_DAY:
			cumulative_day_banner->setSecondaryText(QString("%1 kWh").arg(it.value().toInt()/1000));
			break;
		case EnergyDevice::DIM_CUMULATIVE_MONTH:
			cumulative_month_banner->setSecondaryText(QString("%1 kWh").arg(it.value().toInt()/1000));
			break;
		case EnergyDevice::DIM_CUMULATIVE_YEAR:
			cumulative_year_banner->setSecondaryText(QString("%1 kWh").arg(it.value().toInt()/1000));
			break;
		case EnergyDevice::DIM_CURRENT:
			current_banner->setSecondaryText(QString("%1·kW").arg(it.value().toInt()/1000.0, 0, 'f', 3));
			break;
		case EnergyDevice::ANS_DAILY_AVERAGE_GRAPH:
			assert(it.value().canConvert<GraphData>());
			GraphData *d = new GraphData(it.value().value<GraphData>());
			if (!graph_data_cache.contains(DAILY_AVERAGE))
				graph_data_cache[DAILY_AVERAGE] = new GraphCache;

			GraphCache *cache = graph_data_cache[DAILY_AVERAGE];
			cache->insert(time_period->date(), d);
			break;
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
	current_widget = GRAPH_WIDGET;
	current_graph = static_cast<GraphType>(graph_type);
	current_date = time_period->date();
	widget_container->setCurrentIndex(current_widget);
}

void EnergyView::showBannerWidget()
{
	current_widget = BANNER_WIDGET;
	widget_container->setCurrentIndex(current_widget);
}


QWidget *EnergyView::buildBannerWidget()
{
	QString cumulative_text = tr("Cumulative");

	// Daily page
	cumulative_day_banner = getBanner(this, cumulative_text);
	connect(cumulative_day_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_day_banner, CUMULATIVE_DAY);

	current_banner = getBanner(this, tr("Current"));
	current_banner->nascondi(banner::BUT1);

	QWidget *daily_widget = createWidgetWithVBoxLayout();
	daily_widget->layout()->addWidget(cumulative_day_banner);
	daily_widget->layout()->addWidget(current_banner);

	// Monthly page
	cumulative_month_banner = getBanner(this, cumulative_text);
	connect(cumulative_month_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_month_banner, CUMULATIVE_MONTH);

	daily_av_banner = getBanner(this, tr("Daily Average"));
	connect(daily_av_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(daily_av_banner, DAILY_AVERAGE);

	QWidget *monthly_widget = createWidgetWithVBoxLayout();
	monthly_widget->layout()->addWidget(cumulative_month_banner);
	monthly_widget->layout()->addWidget(daily_av_banner);

	// Yearly page
	cumulative_year_banner = getBanner(this, cumulative_text);
	connect(cumulative_year_banner, SIGNAL(sxClick()), mapper, SLOT(map()));
	mapper->setMapping(cumulative_year_banner, CUMULATIVE_YEAR);

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
	QStackedWidget *w = static_cast<QStackedWidget*>(widget_container->widget(BANNER_WIDGET));
	switch (status)
	{
	case TimePeriodSelection::DAY:
		w->setCurrentIndex(DAILY_PAGE);
		if (QDate::currentDate() == selection_date)
			current_banner->show();
		else
			current_banner->hide();
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
	qWarning("Not yet implemented");
}

