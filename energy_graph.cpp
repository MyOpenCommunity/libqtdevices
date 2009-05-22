#include "energy_graph.h"
#include "fontmanager.h" // bt_global::font
#include "bannfrecce.h"

#include <QSpacerItem>
#include <QBoxLayout>
#include <QGridLayout>
#include <QVariant>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QPen>


namespace
{
// The language used for the floating point number
QLocale loc(QLocale::Italian);
}


EnergyGraph::EnergyGraph()
{
	number_of_bars = 1;
}

void EnergyGraph::init(int bars, QString t, const QMap<int, QString>& x_axis)
{
	text = t;
	number_of_bars = bars;
	custom_x_axis = x_axis;
	graph_data.clear();
	update();
}

#ifdef TEST_ENERGY_GRAPH
void EnergyGraph::generateRandomValues()
{
	for (int i = 1; i <= number_of_bars; ++i)
		graph_data[i] = rand() % 100;
}
#endif

void EnergyGraph::setData(const QMap<int, float> &data)
{
	graph_data = data;
	update();
}

void EnergyGraph::paintEvent(QPaintEvent *e)
{
	const int MARGIN = 5;
	const int SPACING = 3;
	const int AXIS_PEN_WIDTH = 1;

	QPainter p(this);
	if (!graph_data.isEmpty())
	{
		float max_value = -1;
		foreach (float val, graph_data)
			max_value = qMax(max_value, val);

		int left = rect().left() + MARGIN;
		int top = rect().top() + MARGIN;
		int width = rect().width() - MARGIN * 2;
		int height = rect().height() - MARGIN * 2;

		p.setFont(bt_global::font->get(FontManager::SMALLTEXT));
		QFontMetrics fm = p.fontMetrics();

		// Max value on y axis
		QString val = loc.toString(max_value + 0.04999, 'f', 1);
		p.drawText(left, top + fm.height(), val);

		int axis_left = left + fm.width(val) + SPACING;
		int axis_top = top + height - fm.ascent() - SPACING - AXIS_PEN_WIDTH;

		// Draw axis
		p.save();
		QPen pen;

		pen.setStyle(Qt::SolidLine);
		pen.setWidth(AXIS_PEN_WIDTH);
		pen.setColor(QColor("white")); //axis color
		p.setPen(pen);

		// x axis
		p.drawLine(axis_left, axis_top, left + width, axis_top);

		// y axis
		p.drawLine(axis_left, axis_top, axis_left, top);
		p.restore();

		// Descriptive text
		p.drawText(left + width - fm.width(text), top + fm.height(), text);

		int graph_height = axis_top - top - AXIS_PEN_WIDTH - fm.height() - SPACING;
		int graph_width = width - (axis_left + AXIS_PEN_WIDTH - left);

		// calculate the width of each bar
		int bar_width = static_cast<int>(graph_width/ static_cast<float>(number_of_bars));

		// Min & Max values on x axis
		int font_y_pos = axis_top + AXIS_PEN_WIDTH + fm.ascent() + SPACING;

		if (!custom_x_axis.empty())
		{
			QMapIterator<int, QString> it(custom_x_axis);
			while (it.hasNext())
			{
				it.next();
				p.drawText(axis_left + bar_width * it.key() - fm.width(it.value()), font_y_pos, it.value());
			}
		}
		else
		{
			p.drawText(axis_left, font_y_pos, "1");
			QString num = QString::number(number_of_bars);
			p.drawText(axis_left + bar_width * number_of_bars - fm.width(num), font_y_pos, num);
		}

		// draw bars
		p.setPen(QColor("blue"));
		int current_left = axis_left + AXIS_PEN_WIDTH;

		QMapIterator<int, float> it(graph_data);
		while (it.hasNext())
		{
			it.next();
			p.setBrush(it.key() % 2 ? QColor(primaryColor()) : QColor(secondaryColor()));
			float ratio = it.value() / max_value;
			int bar_height = static_cast<int>(ratio * graph_height);
			p.drawRect(QRect(current_left, axis_top - bar_height - AXIS_PEN_WIDTH, bar_width, bar_height));
			current_left += bar_width;
		}
	}
}

QString EnergyGraph::primaryColor()
{
	return _primary_color;
}

QString EnergyGraph::secondaryColor()
{
	return _secondary_color;
}

void EnergyGraph::setSecondaryColor(QString color)
{
	_secondary_color = color;
}

void EnergyGraph::setPrimaryColor(QString color)
{
	_primary_color = color;
}


EnergyTable::EnergyTable(int n_dec)
{
	rows_per_page = 8;
	current_page = 0;
	n_decimal = n_dec;
	main_layout->addSpacing(10);

	date_label = new QLabel;
	date_label->setFont(bt_global::font->get(FontManager::SMALLTEXT));
	main_layout->addWidget(date_label, 0, Qt::AlignCenter);

	createTable();
	QWidget *central_widget = new QWidget;
	QHBoxLayout *l = new QHBoxLayout(central_widget);
	l->setContentsMargins(10, 5, 10, 0);
	l->setSpacing(0);
	l->addWidget(table);

	main_layout->addWidget(central_widget, 1);

	bannFrecce *nav_bar = new bannFrecce(this, 3);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(downClick()), SLOT(pageUp()));
	connect(nav_bar, SIGNAL(upClick()), SLOT(pageDown()));
	main_layout->addWidget(nav_bar);
}

void EnergyTable::setNumDecimal(int n_dec)
{
	n_decimal = n_dec;
}

void EnergyTable::pageUp()
{
	if (current_page > 0)
		--current_page;
	showData();
}

void EnergyTable::pageDown()
{
	if (static_cast<int>((current_page + 1) * rows_per_page) < table_data.count())
		++current_page;
	showData();
}

void EnergyTable::showData()
{
	int start = current_page * rows_per_page;

	QGridLayout *table_layout = static_cast<QGridLayout*>(table->layout());
	QList<int> data_keys = table_data.keys();
	for (int i = 0; i < rows_per_page; ++i)
	{
		QLabel *left = static_cast<QLabel*>(table_layout->itemAtPosition(i + 1, 0)->widget());
		QLabel *right = static_cast<QLabel*>(table_layout->itemAtPosition(i + 1, 1)->widget());
		if (i + start < data_keys.count())
		{
			int key = data_keys.at(i + start);
			int left_value = key;
			if (shift_value != -1)
			{
				left_value = (shift_value + key) % num_values;
				if (left_value == 0)
					left_value = num_values;
			}

			left->setText(QString::number(left_value));
			right->setText(loc.toString(table_data[key], 'f', n_decimal));
		}
		else
		{
			left->setText("");
			right->setText("");
		}
	}
	update();
}

void EnergyTable::createTable()
{
	table = new QFrame;
	QGridLayout *table_layout = new QGridLayout(table);
	table_layout->setContentsMargins(0, 0, 0, 0);
	table_layout->setSpacing(0);
	heading_left = new QLabel;
	heading_left->setAlignment(Qt::AlignCenter);
	heading_left->setFont(bt_global::font->get(FontManager::SMALLTEXT));
	heading_left->setProperty("left", true);
	heading_left->setProperty("heading", true);
	table_layout->addWidget(heading_left, 0, 0);

	heading_right = new QLabel;
	heading_right->setFont(bt_global::font->get(FontManager::SMALLTEXT));
	heading_right->setAlignment(Qt::AlignCenter);
	heading_right->setProperty("right", true);
	heading_right->setProperty("heading", true);
	table_layout->addWidget(heading_right, 0, 1);


	for (int i = 0; i < rows_per_page; ++i)
	{
		QLabel *left = new QLabel;
		left->setFont(bt_global::font->get(FontManager::SMALLTEXT));
		left->setAlignment(Qt::AlignCenter);
		left->setProperty("left", true);
		left->setProperty(i % 2 ? "even" : "odd", true);
		QLabel *right = new QLabel;
		right->setFont(bt_global::font->get(FontManager::SMALLTEXT));
		right->setAlignment(Qt::AlignCenter);
		right->setProperty("right", true);
		right->setProperty(i % 2 ? "even" : "odd", true);

		int row = table_layout->rowCount();
		table_layout->addWidget(left, row, 0);
		table_layout->addWidget(right, row, 1);

	}
}

void EnergyTable::init(int num_val, QString left_text, QString right_text, QString date, int shift_val)
{
	num_values = num_val;
	shift_value = shift_val;
	heading_left->setText(left_text);
	heading_right->setText(right_text);
	date_label->setText(date);
}

void EnergyTable::setData(const QMap<int, float> &data)
{
	table_data.clear();
	int i = 0;
	QMapIterator<int, float> it(data);
	while (it.hasNext() && i < num_values)
	{
		it.next();
		table_data[it.key()] = it.value();
		++i;
	}
	current_page = 0;
	showData();
}
