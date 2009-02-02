#include "energy_view.h"
#include "energy_graph.h"
#include "banner.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName
#include "fontmanager.h" // bt_global::font
#include "bannfrecce.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>

#define ICON_FWD IMG_PATH "btnforward.png"
#define ICON_BACK IMG_PATH "btnbackward.png"
#define ICON_AVANTI IMG_PATH "btnavanti.png"
// TODO: modificare con le icone corrette
#define ICON_GRAPH IMG_PATH "arrrg.png"
#define ICON_CURRENCY IMG_PATH "btncanc.png"


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
	emit timeScaleChanged(status);
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


EnergyView::EnergyView(QString energy_type)
{
	main_layout->setAlignment(Qt::AlignTop);

	// title section
	main_layout->addWidget(getLabel(this, energy_type, FontManager::TEXT), 0, Qt::AlignCenter);

	time_period = new TimePeriodSelection(this);
	connect(time_period, SIGNAL(timeScaleChanged(int)), SLOT(changeTimePeriod(int)));
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
	changeTimePeriod(TimePeriodSelection::DAY);
}

void EnergyView::backClick()
{
	if (current_widget == BANNER_WIDGET)
		emit Closed();
	else
		showBannerWidget();
}

void EnergyView::showGraphWidget()
{
	current_widget = GRAPH_WIDGET;
	widget_container->setCurrentIndex(current_widget);
}

void EnergyView::showBannerWidget()
{
	current_widget = BANNER_WIDGET;
	widget_container->setCurrentIndex(current_widget);
}

QWidget *EnergyView::buildBannerWidget()
{
	QWidget *w = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout;
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
	w->setLayout(main_layout);

	cumulative_banner = getBanner(this, "Cumulative");
	main_layout->addWidget(cumulative_banner);
	connect(cumulative_banner, SIGNAL(sxClick()), SLOT(showGraphWidget()));

	current_banner = getBanner(this, "Current");
	current_banner->nascondi(banner::BUT1);
	main_layout->addWidget(current_banner);

	daily_av_banner = getBanner(this, "Daily Average");
	connect(daily_av_banner, SIGNAL(sxClick()), SLOT(showGraphWidget()));
	main_layout->addWidget(daily_av_banner);
	return w;
}

void EnergyView::changeTimePeriod(int status)
{
	switch (status)
	{
	case TimePeriodSelection::DAY:
		daily_av_banner->hide();
		current_banner->show();
		break;
	case TimePeriodSelection::MONTH:
		daily_av_banner->show();
		current_banner->hide();
		break;
	case TimePeriodSelection::YEAR:
		daily_av_banner->hide();
		current_banner->hide();
		break;
	}
}

void EnergyView::toggleCurrency()
{
	qWarning("Not yet implemented");
}

