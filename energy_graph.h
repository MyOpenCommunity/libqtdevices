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
Q_PROPERTY(QString primary_color READ primaryColor WRITE setPrimaryColor)
Q_PROPERTY(QString secondary_color READ secondaryColor WRITE setSecondaryColor)
Q_PROPERTY(QString border_color READ borderColor WRITE setBorderColor)
public:
	EnergyGraph();

#ifdef TEST_ENERGY_GRAPH
	void generateRandomValues();
#endif

public slots:
	void init(int bars, QString t, const QMap<int, QString> &x_axis);
	void setData(const QMap<int, float> &data);

protected:
	void paintEvent(QPaintEvent *e);

private:
	int number_of_bars;
	QString text;
	QMap<int, QString> custom_x_axis;

	// The attributes to store information set from stylesheet (using the properties)
	QString _primary_color, _secondary_color, _border_color;
	QString primaryColor();
	QString secondaryColor();
	QString borderColor();
	void setSecondaryColor(QString color);
	void setPrimaryColor(QString color);
	void setBorderColor(QString color);

#ifdef TEST_ENERGY_GRAPH
public:
#endif
	QMap<int, float> graph_data;
};


class EnergyTable : public PageLayout
{
Q_OBJECT
public:
	EnergyTable(int n_dec);
	void init(int num_val, QString left_text, QString right_text, QString date, int shift_val=-1);
	void setData(const QMap<int, float> &data);
	void setNumDecimal(int d);

private slots:
	void pageUp();
	void pageDown();

private:
	QLabel *date_label, *heading_left, *heading_right;
	int num_values;
	int shift_value;
	QMap<int, float> table_data;
	QFrame *table;
	int rows_per_page;
	int current_page;
	int n_decimal;
	void createTable();
	void showData();
};


#endif // ENERGY_GRAPH_H
