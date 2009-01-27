#include "energy_view.h"
#include "btbutton.h"
#include "icondispatcher.h" // icons_cache
#include "generic_functions.h" // getPressName
#include "fontmanager.h"

#include <QLabel>
#include <QPainter>
#include <QPen>

#define ICON_FWD IMG_PATH "btnforward.png"
#define ICON_BACK IMG_PATH "btnbackward.png"
#define ICON_AVANTI IMG_PATH "btnavanti.png"

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

QLabel *getLabel(QWidget *parent, QString text, eFontID font)
{
	QFont aFont;
	QLabel *label = new QLabel(parent);
	FontManager::instance()->getFont(font, aFont);
	label->setFont(aFont);
	label->setText(text);
	return label;
}


TimePeriodSelection::TimePeriodSelection(QWidget *parent) :
	QWidget(parent),
	main_layout(this),
	status(DAY),
	selection_date(QDate::currentDate())
{
	back_period = getTrimmedButton(this, ICON_BACK);
	back_period->setAutoRepeat(true);
	connect(back_period, SIGNAL(clicked()), SLOT(periodBackward()));
	main_layout.addWidget(back_period);

	//TODO: questo font e' microscopico, va bene lo stesso? senno' il pulsante in vista giornaliera va fuori
	date_period_label = getLabel(this, selection_date.toString("dd/MM/yy"), font_versio_datiGenFw);
	date_period_label->setAlignment(Qt::AlignCenter);
	main_layout.addWidget(date_period_label);

	forw_period = getTrimmedButton(this, ICON_FWD);
	forw_period->setAutoRepeat(true);
	connect(forw_period, SIGNAL(clicked()), SLOT(periodForward()));
	main_layout.addWidget(forw_period);

	BtButton *btn_cycle;
	btn_cycle = getTrimmedButton(this, ICON_AVANTI);
	connect(btn_cycle, SIGNAL(clicked()), SLOT(changeTimeScale()));
	main_layout.addWidget(btn_cycle);
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

		QFont font;
		FontManager::instance()->getFont(font_versio_datiGenFw, font);
		p.setFont(font);
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
