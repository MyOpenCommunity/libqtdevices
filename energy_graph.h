/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef ENERGY_GRAPH_H
#define ENERGY_GRAPH_H

#include "page.h"

#include <QWidget>
#include <QString>
#include <QColor>
#include <QMap>
#include <QFrame>

class QLabel;


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


class EnergyTableContent : public QFrame
{
Q_PROPERTY(QString border_color   READ borderColor WRITE setBorderColor)
Q_PROPERTY(QString heading_color  READ headingColor WRITE setHeadingColor)
Q_PROPERTY(QString odd_row_color  READ oddRowColor WRITE setOddRowColor)
Q_PROPERTY(QString even_row_color READ evenRowColor WRITE setEvenRowColor)
Q_PROPERTY(QString text_color READ textColor WRITE setTextColor)
Q_OBJECT
public:
	EnergyTableContent(int n_dec);
	void init(int num_val, QString left_text, QString right_text, int shift_val=-1);
	void setData(const QMap<int, float> &data);
	void setNumDecimal(int d);

public slots:
	void pageUp();
	void pageDown();

protected:
	void paintEvent(QPaintEvent *e);

private:
	void setBorderColor(const QString &color);
	void setHeadingColor(const QString &color);
	void setOddRowColor(const QString &color);
	void setEvenRowColor(const QString &color);
	void setTextColor(const QString &color);

	QString borderColor() const;
	QString headingColor() const;
	QString oddRowColor() const;
	QString evenRowColor() const;
	QString textColor() const;

private:
	QString border_color, heading_color, odd_row_color, even_row_color, text_color;
	QMap<int, float> table_data;
	QString left_text, right_text;
	int num_values;
	int shift_value;
	int rows_per_page;
	int current_page;
	int n_decimal;
};


class EnergyTable : public Page
{
Q_OBJECT
public:
	EnergyTable(int n_dec);
	void init(int num_val, QString left_text, QString right_text, QString date, int shift_val=-1);
	void setData(const QMap<int, float> &data);
	void setNumDecimal(int d);

private:
	QLabel *date_label;
	EnergyTableContent *table;
};


#endif // ENERGY_GRAPH_H
