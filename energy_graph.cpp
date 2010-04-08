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
		QString val = loc.toString(max_value + 0.00499, 'f', 2);

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
				float value = i == 0 ? max_value : max_value / 2;
				QString text = loc.toString(value + 0.00499, 'f', 2);
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
			int left = axis_left + AXIS_PEN_WIDTH;
			p.drawText(left + bar_width * (number_of_bars / 2) - fm.width(center) / 2 - bar_width / 2 , font_y_pos, center);

			QString max = QString::number(number_of_bars);
			p.drawText(left + bar_width * number_of_bars - fm.width(max) / 2 - bar_width / 2, font_y_pos, max);

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
				int current_left = left + i * bar_width - bar_width / 2;
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

QString EnergyGraph::axisColor()
{
	return _axis_color;
}

void EnergyGraph::setAxisColor(QString color)
{
	_axis_color = color;
}

QString EnergyGraph::textColor()
{
	return _text_color;
}

void EnergyGraph::setTextColor(QString color)
{
	_text_color = color;
}


EnergyTable::EnergyTable(int n_dec)
{
	main_layout->addSpacing(10);

	date_label = new QLabel;
	date_label->setFont(bt_global::font->get(FontManager::SMALLTEXT));
	main_layout->addWidget(date_label, 0, Qt::AlignCenter);

	table = new EnergyTableContent(n_dec);

	main_layout->addWidget(table, 1);

	bannFrecce *nav_bar = new bannFrecce(this, 3);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(downClick()), table, SLOT(pageUp()));
	connect(nav_bar, SIGNAL(upClick()), table, SLOT(pageDown()));
	main_layout->addWidget(nav_bar);
}

void EnergyTable::init(int num_val, QString left_text, QString right_text,
		       QString date, int shift_val)
{
	date_label->setText(date);
	table->init(num_val, left_text, right_text, shift_val);
}

void EnergyTable::setNumDecimal(int n_dec)
{
	table->setNumDecimal(n_dec);
}

void EnergyTable::setData(const QMap<int, float> &data)
{
	table->setData(data);
}


EnergyTableContent::EnergyTableContent(int n_dec)
{
	setContentsMargins(10, 5, 10, 0);

	rows_per_page = 8;
	current_page = 0;
	n_decimal = n_dec;
}

void EnergyTableContent::init(int num_val, QString _left_text, QString _right_text, int shift_val)
{
	num_values = num_val;
	shift_value = shift_val;
	left_text = _left_text;
	right_text = _right_text;
}

void EnergyTableContent::setData(const QMap<int, float> &data)
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
	update();
}

void EnergyTableContent::setNumDecimal(int n_dec)
{
	n_decimal = n_dec;
}

void EnergyTableContent::pageUp()
{
	if (current_page > 0)
		--current_page;
	update();
}

void EnergyTableContent::pageDown()
{
	if (static_cast<int>((current_page + 1) * rows_per_page) < table_data.count())
		++current_page;
	update();
}

void EnergyTableContent::setBorderColor(const QString &color)
{
	border_color = color;
}

void EnergyTableContent::setHeadingColor(const QString &color)
{
	heading_color = color;
}

void EnergyTableContent::setOddRowColor(const QString &color)
{
	odd_row_color = color;
}

void EnergyTableContent::setEvenRowColor(const QString &color)
{
	even_row_color = color;
}

void EnergyTableContent::setTextColor(const QString &color)
{
	text_color = color;
}

QString EnergyTableContent::borderColor() const
{
	return border_color;
}

QString EnergyTableContent::headingColor() const
{
	return heading_color;
}

QString EnergyTableContent::oddRowColor() const
{
	return odd_row_color;
}

QString EnergyTableContent::evenRowColor() const
{
	return even_row_color;
}

QString EnergyTableContent::textColor() const
{
	return text_color;
}

void EnergyTableContent::paintEvent(QPaintEvent *e)
{
	int mtop, mbottom, mleft, mright;
	getContentsMargins(&mleft, &mtop, &mright, &mbottom);

	QPainter p(this);
	float row_height = float(height() - mtop - mbottom - 2) / float(rows_per_page + 1);
	int row_width = width() - mleft - mright - 3;
	int cell_width = row_width / 2;
	int left = mleft + 1, top = mtop + 1, right_cell_x = left + cell_width + 1;

	QPen pen_border = QPen(QColor(borderColor()));
	QPen pen_head = QPen(QColor(headingColor()));
	QPen pen_odd = QPen(QColor(oddRowColor()));
	QPen pen_even = QPen(QColor(evenRowColor()));
	QPen pen_text = QPen(QColor(textColor()));
	QBrush brush_head = QBrush(QColor(headingColor()));
	QBrush brush_odd = QBrush(QColor(oddRowColor()));
	QBrush brush_even = QBrush(QColor(evenRowColor()));

	p.setFont(bt_global::font->get(FontManager::SMALLTEXT));

	// external border
	p.setPen(pen_border);
	p.setBrush(QBrush());
	p.drawRect(mleft, mtop, row_width + 2, height() - mtop - mbottom - 1);

	// header
	p.setPen(pen_head);
	p.setBrush(brush_head);
	p.drawRect(left, top, row_width, row_height);
	p.setPen(pen_text);
	p.drawText(QRectF(left, top, cell_width, row_height),
		   Qt::AlignCenter, left_text);
	p.drawText(QRectF(right_cell_x, top, cell_width, row_height),
		   Qt::AlignCenter, right_text);

	// cells
	QList<int> data_keys = table_data.keys();
	int start = current_page * rows_per_page;
	for (int i = 0; i < rows_per_page; ++i)
	{
		int row_y = top + row_height + i * row_height;

		p.setPen(i & 1 ? pen_even : pen_odd);
		p.setBrush(i & 1 ? brush_even : brush_odd);
		p.drawRect(left, row_y, row_width, row_height);

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

			p.setPen(pen_text);
			p.drawText(QRectF(left, row_y, cell_width, row_height),
				   Qt::AlignCenter, QString::number(left_value));
			p.drawText(QRectF(right_cell_x, row_y, cell_width, row_height),
				   Qt::AlignCenter, loc.toString(table_data[key], 'f', n_decimal));
		}
	}

	// central separator line
	p.setPen(pen_border);
	p.drawLine(right_cell_x - 1, mtop, right_cell_x - 1, height() - mbottom - mtop);
}
