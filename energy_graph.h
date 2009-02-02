#ifndef ENERGY_GRAPH_H
#define ENERGY_GRAPH_H

#include <QWidget>


class EnergyGraph : public QWidget
{
Q_OBJECT
public:
	EnergyGraph();
	void clearData();
	void setColors(QColor first, QColor second = QColor());

public slots:
	// assume that all values are positive
	void setGraphData(QVector<int> data);
	void setNumberOfBars(int num);

protected:
	void paintEvent(QPaintEvent *e);
	void showEvent(QShowEvent *e);

private:
	int findMax();
	void generateRandomValues();
	unsigned number_of_bars;
	QVector<int> graph_data;
	int max_value;
	QColor primary_color, secondary_color;
};


#endif // ENERGY_GRAPH_H
