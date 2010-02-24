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


#ifndef ITEMS_H
#define ITEMS_H

// This file contains widgets with some logic that are not banner.


#include <QWidget>
#include <QString>

class BtButton;


// A simple class to save and restore the status of a ItemTuning. This class
// has no public method or attribute, because it is designed to preserve the
// encapsulation of a ItemTuning (as the Memento Pattern).
class ItemTuningStatus
{
friend class ItemTuning;
private:
	int current_level;
};


/**
 * This class contains an item like with two buttons (like a banner, but
 * without a fixed width) for tuning.
 */
class ItemTuning : public QWidget
{
Q_OBJECT
public:
	ItemTuning(QString text, QString icon, QWidget *parent=0);

	ItemTuningStatus getStatus();
	void setStatus(const ItemTuningStatus &st);

public slots:
	void decreaseLevel();
	void increaseLevel();

signals:
	void valueChanged(int);

private slots:
	void changeIcons();

private:
	int current_level;
	QString icon_name;
	BtButton *left, *right;
};

#endif // ITEMS_H
