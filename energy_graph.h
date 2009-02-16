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
	void generateRandomValues();

public slots:
	void init(int bars, QString t);
	void setData(const QMap<int, int> &data);


protected:
	void paintEvent(QPaintEvent *e);
	void showEvent(QShowEvent *e);

private:
	int findMax();

	int number_of_bars;
	QMap<int, int> graph_data;
	int max_value;
	QString text;
	QColor primary_color, secondary_color;
};


#endif // ENERGY_GRAPH_H
