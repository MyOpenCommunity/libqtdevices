#include "energy_view.h"
#include "banner.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName
#include "fontmanager.h" // bt_global::font
#include "bannfrecce.h"

#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QHBoxLayout>
#include <QVBoxLayout>

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


GraphWidget::GraphWidget(QWidget *parent) :
	QWidget(parent),
	number_of_bars(1),
	// dummy values
	graph_data(QVector<int>(10, 10)),
	max_value(findMax()),
	primary_color(QColor("white")),
	secondary_color(primary_color)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	// TODO: read these from config file
	setColors(QColor("cyan"), QColor("white"));
}

void GraphWidget::setNumberOfBars(int num)
{
	number_of_bars = num;
	generateRandomValues();
	update();
}

void GraphWidget::generateRandomValues()
{
	// TODO: just for the demo, fill in the graph with random values
	for (QVector<int>::size_type i = 0; i < graph_data.size(); ++i)
		graph_data[i] = rand();
	max_value = findMax();
}

int GraphWidget::findMax()
{
	int max = -1;
	for (QVector<int>::size_type i = 0; i < graph_data.size(); ++i)
		max = qMax(max, graph_data[i]);
	return max;
}

void GraphWidget::setGraphData(QVector<int> data)
{
	graph_data = data;
	max_value = findMax();
}

void GraphWidget::showEvent(QShowEvent *e)
{
	generateRandomValues();
	update();
}

void GraphWidget::setColors(QColor first, QColor second)
{
	primary_color = first;
	second.isValid() ?
		secondary_color = second :
		secondary_color = primary_color;
}

void GraphWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	if (graph_data.isEmpty())
	{
		// draw a text "loading" on it
	}
	else
	{
		int x = rect().x();
		int y = rect().y();
		int remaining_height = rect().height();
		int remaining_width = rect().width();

		p.setFont(bt_global::font.get(FontManager::SMALLTEXT));
		QFontMetrics fm = p.fontMetrics();

		//
		// draw right legend
		p.save();
		int font_y = remaining_height / 2;
		int font_x = 0;
		font_x += fm.ascent();
		p.translate(font_x, font_y);
		p.rotate(-90);
		p.drawText(0, 0, tr("kWh"));
		remaining_width -= fm.height();
		p.restore();

		// draw bottom legend
		font_y = remaining_height;
		font_y -= fm.descent();
		// for now, just draw top and bottom of scale (ie, 1-24 for hours, 1-30 for days...)
		// bottom of scale
		p.drawText(rect().width() - remaining_width, font_y, "1");
		QString num = QString::number(number_of_bars);
		p.drawText(remaining_width - fm.width(num), font_y, num);

		remaining_height -= fm.height();

		// 3. draw axis
		p.save();
		QPen pen;
#define AXIS_PEN_WIDTH 2
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(AXIS_PEN_WIDTH);
		pen.setColor(QColor("white")); //axis color
		p.setPen(pen);
		// x axis
		int y_axis = remaining_height;
		p.drawLine(rect().width() - remaining_width, y_axis, rect().width(), y_axis);
		remaining_height -= AXIS_PEN_WIDTH;
		// y axis
		int x_axis = rect().width() - remaining_width;
		p.drawLine(x_axis, y_axis, x_axis, y);
		remaining_width -= AXIS_PEN_WIDTH;
		p.restore();

		// calculate the width of each bar
		float bar_width = static_cast<float>(remaining_width) / number_of_bars;

		// draw bars
		p.setPen(QColor("blue"));
		x = rect().width() - remaining_width;
		for (QVector<int>::size_type i = 0; i < graph_data.size(); ++i)
		{
			(i % 2) ? p.setBrush(primary_color) : p.setBrush(secondary_color);
			float ratio = graph_data[i] / static_cast<float>(max_value);
			int bar_height = static_cast<int>(ratio * remaining_height);
			QRect tmp = QRect(x, remaining_height - bar_height, static_cast<int>(bar_width), bar_height);
			x += static_cast<int>(bar_width);
			p.drawRect(tmp);
		}
	}
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


EnergyView::EnergyView()
{
	nav_bar = new bannFrecce(this, 10, ICON_CURRENCY);
	nav_bar->setGeometry(0 , MAX_HEIGHT - MAX_HEIGHT/NUM_RIGHE, MAX_WIDTH ,MAX_HEIGHT/NUM_RIGHE);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	main_layout->setAlignment(Qt::AlignTop);

	// title section
	const QString name = "Electricity";
	main_layout->addWidget(getLabel(this, name, FontManager::TEXT), 0, Qt::AlignCenter);

	time_period = new TimePeriodSelection(this);
	main_layout->addWidget(time_period);

	// energy data section
	cumulative_banner = getBanner(this, "Cumulative");
	main_layout->addWidget(cumulative_banner);
	connect(cumulative_banner, SIGNAL(sxClick()), SIGNAL(showGraph()));
	current_banner = getBanner(this, "Current");
	current_banner->nascondi(banner::BUT1);
	main_layout->addWidget(current_banner);
	daily_av_banner = getBanner(this, "Daily Average");
	connect(daily_av_banner, SIGNAL(sxClick()), SIGNAL(showGraph()));
	daily_av_banner->hide();
	main_layout->addWidget(daily_av_banner);
}

void EnergyView::changeTimePeriod(int status)
{
	switch (status)
	{
	case TimePeriodSelection::DAY:
		setDayPeriod();
		break;
	case TimePeriodSelection::MONTH:
		setMonthPeriod();
		break;
	case TimePeriodSelection::YEAR:
		setYearPeriod();
		break;
	}
}

void EnergyView::toggleCurrency()
{
	qWarning("Not yet implemented");
}

void EnergyView::setDayPeriod()
{
	cumulative_banner->show();
	daily_av_banner->hide();
	current_banner->show();
}

void EnergyView::setMonthPeriod()
{
	cumulative_banner->show();
	daily_av_banner->show();
	current_banner->hide();
}

void EnergyView::setYearPeriod()
{
	cumulative_banner->show();
	daily_av_banner->hide();
	current_banner->hide();
}
