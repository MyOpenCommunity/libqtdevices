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


#include "radio.h"
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "navigation_bar.h" // NavigationBar
#include "hardware_functions.h" // beep()

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

namespace
{

	QWidget *composeButtons(BtButton *left, BtButton *right)
	{
		QWidget *w = new QWidget;
		QHBoxLayout *hbox = new QHBoxLayout(w);
		hbox->setContentsMargins(0, 0, 0, 0);
		hbox->setSpacing(10);
		hbox->addWidget(left);
		hbox->addWidget(right);
		return w;
	}
}

RadioInfo::RadioInfo() : QWidget(0)
{
	// background image will be set using stylesheets
	setFixedHeight(50);
	setFixedWidth(452);

	QGridLayout *grid = new QGridLayout(this);
	grid->setContentsMargins(10, 0, 5, 0);
	grid->setSpacing(0);

	radio_name = new QLabel;
	radio_name->setText("Virgin radio");
	radio_name->setFont(bt_global::font->get(FontManager::RADIO_NAME));
	grid->addWidget(radio_name, 0, 0, 1, 1, Qt::AlignCenter);

	channel = new QLabel;
	channel->setText(tr("Channel. -"));
	channel->setFont(bt_global::font->get(FontManager::RADIO_MEMORY_NUMBER));
	grid->addWidget(channel, 1, 0, 1, 1, Qt::AlignRight);

	frequency = new QLabel;
	frequency->setText("FM 123.75");
	frequency->setFont(bt_global::font->get(FontManager::RADIO_STATION));
	grid->addWidget(frequency, 1, 0, 1, 1, Qt::AlignLeft);
}

void RadioInfo::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

#define MEMORY_PRESS_TIME 3000

radio::radio(const QString &amb)
{
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	buildPage(createContent(), nav_bar, amb);
	memory_number = 0;
	memory_timer.setInterval(MEMORY_PRESS_TIME);
	memory_timer.setSingleShot(true);
	connect(&memory_timer, SIGNAL(timeout()), SLOT(storeMemoryStation()));
}

QWidget *radio::createContent()
{
	QWidget *content = new QWidget;

	// radio description, with frequency and memory station
	RadioInfo *name_box = new RadioInfo;

	// tuning control, manual/auto buttons
	minus_button = new BtButton;
	plus_button = new BtButton;
	auto_button = new BtButton;
	manual_button = new BtButton;

	plus_button->setImage(bt_global::skin->getImage("plus"));
	minus_button->setImage(bt_global::skin->getImage("minus"));
	manual_off = bt_global::skin->getImage("man_off");
	manual_on = bt_global::skin->getImage("man_on");
	auto_off = bt_global::skin->getImage("auto_off");
	auto_on = bt_global::skin->getImage("auto_on");
	manual_button->setImage(manual_off);
	auto_button->setImage(auto_on);

	connect(auto_button,SIGNAL(clicked()),this,SLOT(setAuto()));
	connect(manual_button,SIGNAL(clicked()),this,SLOT(setMan()));
	connect(minus_button,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	connect(plus_button,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));

	QGridLayout *tuning = new QGridLayout;
	tuning->setContentsMargins(0, 0, 0, 0);
	tuning->setSpacing(0);
	tuning->addWidget(composeButtons(minus_button, plus_button), 0, 0);
	tuning->setColumnStretch(1, 1);
	tuning->addWidget(composeButtons(auto_button, manual_button), 0, 2);

	// memory numbers buttons
	QList<BtButton *> buttons;
	QHBoxLayout *memory = new QHBoxLayout;
	memory->setContentsMargins(0, 0, 0, 0);
	memory->setSpacing(10);
	memory->addStretch(1);
	for (int i = 0; i < 5; ++i)
	{
		BtButton *b = new BtButton;
		QString str = QString("num_%1").arg(i + 1);
		b->setImage(bt_global::skin->getImage(str));
		buttons << b;
		// Each button must be associated with the memory location, which go 1-5 instead of 0-4
		button_group.addButton(b, i + 1);
		memory->addWidget(b);
	}
	// below it's not right. If the user presses the button for a long time, the station is saved.
	// On click, change memory station
	connect(&button_group, SIGNAL(buttonClicked(int)), SLOT(changeStation(int)));
	connect(&button_group, SIGNAL(buttonPressed(int)), SLOT(memoryButtonPressed(int)));
	connect(&button_group, SIGNAL(buttonReleased(int)), SLOT(memoryButtonReleased(int)));
	memory->addStretch(1);

	// add everything to layout
	QVBoxLayout *main = new QVBoxLayout(content);
	main->setContentsMargins(10, 0, 10, 45);
	main->setSpacing(0);
	main->addWidget(name_box);
	main->addLayout(tuning);
	main->addLayout(memory);

	manual=false;

	return content;
}

void radio::setFreq(float f)
{
	frequenza = f;
	QString fr;
	fr.sprintf("%.2f",frequenza);
	freq->setText(fr);
}

float radio::getFreq()
{
	return frequenza;
}

void radio::setRDS(const QString & s)
{
	QString rds = s;
	rds.truncate(8);
	rdsLabel->setText(rds);
}

void radio::setStaz(uchar st)
{
	if (st)
		progrText->setText(QString::number((int)st)+":");
	else
		progrText->setText("--:");
}

void radio::setAmbDescr(const QString & d)
{
	ambDescr->setText(d);
}

void radio::setName(const QString & s)
{
	radioName->setText(s);
}

void radio::changeStation(int station_num)
{
	beep();
	qDebug("Changing to station number: %d", station_num);
}

void radio::memoryButtonPressed(int but_num)
{
	memory_timer.start();
	memory_number = but_num;
	qDebug("Memory button pressed: %d", but_num);
}

void radio::memoryButtonReleased(int but_num)
{
	memory_timer.stop();
	qDebug("Memory button released: %d", but_num);
}

void radio::storeMemoryStation()
{
	emit memoFreq((uchar) memory_number);
	qDebug("Storing frequency to memory station %d", memory_number);
}

void radio::setAuto()
{
	connect(minus_button,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	connect(plus_button,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	disconnect(minus_button,SIGNAL(clicked()),this,SIGNAL(decFreqMan()));
	disconnect(plus_button,SIGNAL(clicked()),this,SIGNAL(aumFreqMan()));
	disconnect(plus_button,SIGNAL(clicked()),this,SLOT(verTas()));
	disconnect(minus_button,SIGNAL(clicked()),this,SLOT(verTas()));
	plus_button->setAutoRepeat (false);
	minus_button->setAutoRepeat (false);
	if (manual)
	{
		manual = false;
		manual_button->setImage(manual_off);
		auto_button->setImage(auto_on);
	}
}

void radio::setMan()
{
	disconnect(minus_button,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	disconnect(plus_button,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	connect(minus_button,SIGNAL(clicked()),this,SIGNAL(decFreqMan()));
	connect(plus_button,SIGNAL(clicked()),this,SIGNAL(aumFreqMan()));
	plus_button->setAutoRepeat (true);
	minus_button->setAutoRepeat (true);
	connect(plus_button,SIGNAL(clicked()),this,SLOT(verTas()));
	connect(minus_button,SIGNAL(clicked()),this,SLOT(verTas()));
	if (!manual)
	{
		manual = true;
		manual_button->setImage(manual_on);
		auto_button->setImage(auto_off);
	}
}

void radio::memo(int memory)
{
	emit(memoFreq(uchar(memory)));
}

void radio::verTas()
{
	if ((!plus_button->isDown()) && (!minus_button->isDown()))
		emit (richFreq());
}
