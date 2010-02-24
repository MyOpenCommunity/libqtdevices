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


#include "items.h"

#include "btbutton.h"
#include "generic_functions.h" // getBostikName

#include <QLabel>
#include <QBoxLayout>


ItemTuning::ItemTuning(QString text, QString icon, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	main_layout->setSizeConstraint(QLayout::SetFixedSize);

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->setSizeConstraint(QLayout::SetFixedSize);

	left = new BtButton;
	connect(left, SIGNAL(clicked()), SLOT(decreaseLevel()));
	hlayout->addWidget(left);

	right = new BtButton;
	connect(right, SIGNAL(clicked()), SLOT(increaseLevel()));
	hlayout->addWidget(right);

	main_layout->addLayout(hlayout);

	if (!text.isEmpty())
	{
		QLabel *label = new QLabel;
		label->setText(text);
		main_layout->addWidget(label, 1, Qt::AlignHCenter);
	}

	// levels go from 0 to 8 inclusive
	current_level = 4;
	icon_name = icon;

	left->setImage(getBostikName(icon_name, QString("sxl") + QString::number(current_level)));
	right->setImage(getBostikName(icon_name, QString("dxl") + QString::number(current_level)));
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(changeIcons()));
}

ItemTuningStatus ItemTuning::getStatus()
{
	ItemTuningStatus st;
	st.current_level = current_level;
	return st;
}

void ItemTuning::setStatus(const ItemTuningStatus &st)
{
	current_level = st.current_level;
	changeIcons();
}

void ItemTuning::decreaseLevel()
{
	if (current_level > 0)
	{
		--current_level;
		emit valueChanged(current_level);
	}
}

void ItemTuning::increaseLevel()
{
	if (current_level < 8)
	{
		++current_level;
		emit valueChanged(current_level);
	}
}

void ItemTuning::changeIcons()
{
	left->setImage(getBostikName(icon_name, QString("sxl") + QString::number(current_level)));
	right->setImage(getBostikName(icon_name, QString("dxl") + QString::number(current_level)));
}

