#ifndef ENERGY_GRAPH_H
#define ENERGY_GRAPH_H

#include <QWidget>
#include <QString>
#include <QColor>
#include <QMap>


class EnergyGraph : public QWidget
{
Q_OBJECT
public:
	EnergyGraph();

#ifdef TEST_ENERGY_GRAPH
	void generateRandomValues();
#endif

public slots:
	void init(int bars, QString t);
	void setData(const QMap<int, float> &data);


protected:
	void paintEvent(QPaintEvent *e);

private:
	int number_of_bars;
	QMap<int, float> graph_data;
	QString text;
	QColor primary_color, secondary_color;
};


#endif // ENERGY_GRAPH_H
