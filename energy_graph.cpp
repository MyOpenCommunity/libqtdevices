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

		// Coordinates have the origin in the point on the top left of the display
		int left = rect().left() + MARGIN;
		int top = rect().top() + MARGIN;
		int width = rect().width() - MARGIN * 2;
		int height = rect().height() - MARGIN * 2;

		p.setFont(bt_global::font->get(FontManager::SMALLTEXT));
		QFontMetrics fm = p.fontMetrics();

		// We use the max value as to calculate the shift on the x axis
		QString val = loc.toString(max_value + 0.04999, 'f', 1);

		int axis_left = left + fm.width(val) + SPACING * 2;
		int axis_top = top + height - fm.ascent() - SPACING - AXIS_PEN_WIDTH;

		QPen pen_axis;
		pen_axis.setStyle(Qt::SolidLine);
		pen_axis.setWidth(AXIS_PEN_WIDTH);
		pen_axis.setColor(QColor("white")); //axis color

		// Calculate the heigth and width of the graph
		int graph_height = axis_top - top - AXIS_PEN_WIDTH - fm.height() - SPACING;
		int graph_width = width - (axis_left + AXIS_PEN_WIDTH - left);

		// Draw axis
		QPen old_pen = p.pen();
		p.setPen(pen_axis);

		// x axis
		p.drawLine(axis_left, axis_top, left + width, axis_top);

		// y axis
		p.drawLine(axis_left, axis_top, axis_left, top);

		// Draw the dashes and the text in the y axis.
		int y_max_value = top + fm.height() + SPACING + AXIS_PEN_WIDTH;
		int quarter = (axis_top - y_max_value) / 4;

		for (int i = 0; i < 4; ++i)
		{
			if (i == 0 || i == 2)
			{
				int value = i == 0 ? max_value : max_value / 2;
				QString text = loc.toString(value + 0.04999, 'f', 1);
				p.drawText(left, y_max_value + quarter*i + fm.ascent() / 2, text);
			}
			p.drawLine(axis_left, y_max_value + quarter*i, axis_left - MARGIN, y_max_value + quarter*i);
		}

		p.setPen(old_pen);

		// Descriptive text
		p.drawText(left + width - fm.width(text), top + fm.height(), text);

		// calculate the width of each bar
		int bar_width = static_cast<int>(graph_width / static_cast<float>(number_of_bars));

		// Min & Max values on x axis
		int font_y_pos = axis_top + AXIS_PEN_WIDTH + fm.ascent() + SPACING;

		if (!custom_x_axis.empty())
		{
			QMapIterator<int, QString> it(custom_x_axis);
			while (it.hasNext())
			{
				it.next();
				int current_left = axis_left + AXIS_PEN_WIDTH + bar_width * it.key();
				p.drawText(current_left - bar_width + (bar_width - fm.width(it.value())) / 2, font_y_pos, it.value());

				QPen old_pen = p.pen();
				p.setPen(pen_axis);
				p.drawLine(current_left, axis_top, current_left, axis_top + MARGIN);
				p.setPen(old_pen);
			}
		}
		else
		{
			p.drawText(axis_left, font_y_pos, "1");
			QString center = QString::number(number_of_bars / 2);
			p.drawText(axis_left + bar_width * number_of_bars / 2 - fm.width(center) / 2, font_y_pos, center);

			QString max = QString::number(number_of_bars);
			p.drawText(axis_left + bar_width * number_of_bars - fm.width(max), font_y_pos, max);

			// Draw a line for each quarter of the graph.
			QList<int> lines;
			lines.append(number_of_bars / 4);
			lines.append(number_of_bars / 2);
			lines.append(number_of_bars / 4 * 3);
			lines.append(number_of_bars);

			QPen old_pen = p.pen();
			p.setPen(pen_axis);

			foreach (int i, lines)
			{
				int current_left = axis_left + i * bar_width + AXIS_PEN_WIDTH;
				p.drawLine(current_left, axis_top, current_left, axis_top + MARGIN);
			}

			p.setPen(old_pen);
		}

		// draw bars
		p.setPen(_border_color);
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

QString EnergyGraph::borderColor()
{
	return _border_color;
}

void EnergyGraph::setBorderColor(QString color)
{
	_border_color = color;
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
