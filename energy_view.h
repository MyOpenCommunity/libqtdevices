#ifndef ENERGY_VIEW_H
#define ENERGY_VIEW_H

#include <QColor>
#include <QDate>
#include <QHBoxLayout>
#include <QVector>
#include <QWidget>

class QLabel;
class BtButton;

class TimePeriodSelection : public QWidget
{
Q_OBJECT
public:
	enum TimePeriod
	{
		DAY,
		MONTH,
		YEAR,
	};
	TimePeriodSelection(QWidget *parent);
	QDate date();

private slots:
	void changeTimeScale();
	void periodForward();
	void periodBackward();

private:
	/**
	 * Changes the time period considered based on current status and the delta provided, ie.
	 * if status is DAY then delta days are added to selection_date.
	 * \param delta The number of days or months to be added to selection_date
	 */
	void changeTimePeriod(int delta);
	void setDate(QDate new_date);

	QHBoxLayout main_layout;
	TimePeriod status;
	BtButton *back_period, *forw_period;
	QLabel *date_period_label;
	QDate selection_date;

signals:
	void timeScaleChanged(int);
};


class GraphWidget : public QWidget
{
Q_OBJECT
public:
	GraphWidget(QWidget *parent);
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

#endif // ENERGY_VIEW_H
