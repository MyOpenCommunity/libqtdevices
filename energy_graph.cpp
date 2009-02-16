#include "energy_graph.h"
#include "fontmanager.h" // bt_global::font

#include <QPainter>
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

void EnergyGraph::setData(const QMap<int, int> &data)
{
	graph_data = data;
	max_value = findMax();
	update();
}

void EnergyGraph::generateRandomValues()
{
	// TODO: just for the demo, fill in the graph with random values
	for (int i = 0; i < number_of_bars; ++i)
		graph_data[i] = rand();
	max_value = findMax();
}

int EnergyGraph::findMax()
{
	int max = -1;
	foreach (int val, graph_data)
		max = qMax (max, val);

	return max;
}

void EnergyGraph::showEvent(QShowEvent *e)
{
	update();
}

void EnergyGraph::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	if (!graph_data.isEmpty())
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

		p.drawText(remaining_width - fm.width(text), fm.height(), text);
		// We leave some space between text and bars
		int text_space = fm.height()*3/2;

		// 3. draw axis
		p.save();
		QPen pen;
#define AXIS_PEN_WIDTH 1
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
		int bar_width = static_cast<int>(remaining_width / static_cast<float>(number_of_bars));

		// draw bars
		p.setPen(QColor("blue"));
		x = rect().width() - remaining_width;

		QMapIterator<int, int> it(graph_data);
		while (it.hasNext())
		{
			it.next();
			p.setBrush(it.key() % 2 ? primary_color : secondary_color);
			float ratio = it.value() / static_cast<float>(max_value);
			int bar_height = static_cast<int>(ratio * (remaining_height  - text_space));
			QRect tmp = QRect(x, remaining_height - bar_height, bar_width, bar_height);
			x += bar_width;
			p.drawRect(tmp);
		}
	}
}

