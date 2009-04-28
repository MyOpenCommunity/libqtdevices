#include "energy_graph.h"
#include "fontmanager.h" // bt_global::font

#include <QPainter>
#include <QDebug>
#include <QPen>


EnergyGraph::EnergyGraph() : primary_color("#1449C8"), secondary_color("#3587FF")
{
	number_of_bars = 1;
}

void EnergyGraph::init(int bars, QString t)
{
	text = t;
	number_of_bars = bars;
	graph_data.clear();
	update();
}

#ifdef TEST_ENERGY_GRAPH
void EnergyGraph::generateRandomValues()
{
	for (int i = 0; i < number_of_bars; ++i)
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
		QString val = QString::number(max_value);
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

		// Min & Max values on x axis
		int font_y_pos = axis_top + AXIS_PEN_WIDTH + fm.ascent() + SPACING;
		p.drawText(axis_left, font_y_pos, "1");
		QString num = QString::number(number_of_bars);
		p.drawText(left + width - fm.width(num), font_y_pos, num);

		// Descriptive text
		p.drawText(left + width - fm.width(text), top + fm.height(), text);

		int graph_height = axis_top - top - AXIS_PEN_WIDTH - fm.height() - SPACING;
		int graph_width = width - (axis_left + AXIS_PEN_WIDTH - left);

		// calculate the width of each bar
		int bar_width = static_cast<int>(graph_width/ static_cast<float>(number_of_bars));

		// draw bars
		p.setPen(QColor("blue"));
		int current_left = axis_left + AXIS_PEN_WIDTH;

		QMapIterator<int, float> it(graph_data);
		while (it.hasNext())
		{
			it.next();
			p.setBrush(it.key() % 2 ? primary_color : secondary_color);
			float ratio = it.value() / max_value;
			int bar_height = static_cast<int>(ratio * graph_height);
			p.drawRect(QRect(current_left, axis_top - bar_height - AXIS_PEN_WIDTH, bar_width, bar_height));
			current_left += bar_width;
		}
	}
}


EnergyTable::EnergyTable()
{
	addBackButton();
}

void EnergyTable::init(int num_values, QString date)
{
}

void EnergyTable::setData(const QMap<int, float> &data)
{
}
