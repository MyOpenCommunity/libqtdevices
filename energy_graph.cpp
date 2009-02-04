#include "energy_graph.h"
#include "fontmanager.h" // bt_global::font

#include <QPainter>
#include <QPen>


EnergyGraph::EnergyGraph() : number_of_bars(1),
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

void EnergyGraph::setNumberOfBars(int num)
{
	number_of_bars = num;
	generateRandomValues();
	update();
}

void EnergyGraph::generateRandomValues()
{
	// TODO: just for the demo, fill in the graph with random values
	for (QVector<int>::size_type i = 0; i < graph_data.size(); ++i)
		graph_data[i] = rand();
	max_value = findMax();
}

int EnergyGraph::findMax()
{
	int max = -1;
	for (QVector<int>::size_type i = 0; i < graph_data.size(); ++i)
		max = qMax(max, graph_data[i]);
	return max;
}

void EnergyGraph::setGraphData(QVector<int> data)
{
	graph_data = data;
	max_value = findMax();
}

void EnergyGraph::showEvent(QShowEvent *e)
{
	generateRandomValues();
	update();
}

void EnergyGraph::setColors(QColor first, QColor second)
{
	primary_color = first;
	second.isValid() ?
		secondary_color = second :
		secondary_color = primary_color;
}

void EnergyGraph::paintEvent(QPaintEvent *e)
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

