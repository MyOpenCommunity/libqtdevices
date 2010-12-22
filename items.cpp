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
#include "fontmanager.h"
#include "audiostatemachine.h" // VOLUME_MAX, VOLUME_MIN, DEFAULT_VOLUME

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
		label->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
		main_layout->addWidget(label, 1, Qt::AlignHCenter);
	}

	current_level = DEFAULT_VOLUME;
	icon_name = icon;
	enabled = true;

	left->setImage(getBostikName(icon_name, QString("sxl") + QString::number(current_level)));
	right->setImage(getBostikName(icon_name, QString("dxl") + QString::number(current_level)));
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(changeIcons()));
}

ItemTuningStatus ItemTuning::getStatus() const
{
	ItemTuningStatus st;
	st.current_level = current_level;
	return st;
}

void ItemTuning::enableBeep(bool enable)
{
	left->enableBeep(enable);
	right->enableBeep(enable);
}

void ItemTuning::setStatus(const ItemTuningStatus &st)
{
	setLevel(st.current_level);
}

void ItemTuning::setLevel(int level)
{
	Q_ASSERT_X(level >= VOLUME_MIN && level <= VOLUME_MAX, "ItemTuning::setLevel",
		"level out of range!");

	current_level = level;
	changeIcons();
}

int ItemTuning::level()
{
	return current_level;
}

void ItemTuning::enable()
{
	enabled = true;
}

void ItemTuning::disable()
{
	enabled = false;
}

void ItemTuning::decreaseLevel()
{
	if (current_level > VOLUME_MIN && enabled)
	{
		--current_level;
		emit valueChanged(current_level);
	}
}

void ItemTuning::increaseLevel()
{
	if (current_level < VOLUME_MAX && enabled)
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

