#ifndef ENERGY_GRAPH_H
#define ENERGY_GRAPH_H

#include <QWidget>
#include <QMap>


class EnergyGraph : public QWidget
{
Q_OBJECT
public:
	EnergyGraph();
	void setColors(QColor first, QColor second = QColor());

public slots:
	void setData(const QMap<int, int> &data);
	void setNumberOfBars(int num);

protected:
	void paintEvent(QPaintEvent *e);
	void showEvent(QShowEvent *e);

private:
	int findMax();
	void generateRandomValues();
	int number_of_bars;
	QMap<int, int> graph_data;
	int max_value;
	QColor primary_color, secondary_color;
};


#endif // ENERGY_GRAPH_H
