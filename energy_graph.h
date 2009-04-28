#ifndef ENERGY_GRAPH_H
#define ENERGY_GRAPH_H

#include "page.h"

#include <QWidget>
#include <QString>
#include <QColor>
#include <QMap>

class QLabel;
class QFrame;


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
public:
	QMap<int, float> graph_data;
	QString text;
	QColor primary_color, secondary_color;
};


class EnergyTable : public PageLayout
{
Q_OBJECT
public:
	EnergyTable();
	void init(QString left_text, QString right_text, QString date);
	void setData(const QMap<int, float> &data);

private slots:
	void pageUp();
	void pageDown();

private:
	QLabel *date_label, *heading_left, *heading_right;
	int num_values;
	QMap<int, float> table_data;
	QFrame *table;
	int rows_per_page;
	int current_page;
	void createTable();
	void showData();
};


#endif // ENERGY_GRAPH_H
