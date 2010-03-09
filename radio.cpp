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
#include "bannfrecce.h"
#include "btbutton.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "navigation_bar.h" // NavigationBar

#include <QLabel>
#include <QLCDNumber>
#include <QDebug>
#include <QVBoxLayout>
#include <QStackedWidget>

radio::radio(const QString &amb)
{
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SLOT(handleClose()));
	state = STATION_SELECTION;

	buildPage(createContent(amb), nav_bar);
}

QWidget *radio::createContent(const QString &amb)
{
	QWidget *content = new QWidget;

	// top of all, radio description
	radioName = new QLabel;
	radioName->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	radioName->setFont(bt_global::font->get(FontManager::SMALLTEXT));

	// below radio name, environment description
	ambDescr = new QLabel;
	ambDescr->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
	ambDescr->setFont(bt_global::font->get(FontManager::SMALLTEXT));
	ambDescr->setText(amb);

	// below description, frequency + cycle button
	QGridLayout *freq_layout = new QGridLayout;
	freq_layout->setContentsMargins(0, 0, 0, 0);
	freq_layout->setSpacing(0);
	cicBut = new BtButton;
	cicBut->setImage(bt_global::skin->getImage("cycle"));
	connect(cicBut,SIGNAL(clicked()),this,SIGNAL(changeStaz()));
	freq_layout->addWidget(cicBut, 0, 0);
	freq_layout->setColumnStretch(0, 1);
	freq_layout->setColumnMinimumWidth(0, cicBut->minimumSizeHint().width());
	freq_layout->setRowMinimumHeight(0, cicBut->minimumSizeHint().height());

	progrText = new QLabel;
	progrText->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	progrText->setFont(bt_global::font->get(FontManager::TEXT));
	freq_layout->addWidget(progrText, 0, 1);

	freq = new QLCDNumber;
	freq->setSegmentStyle(QLCDNumber::Flat);
	freq->setSmallDecimalPoint(true);
	freq->setNumDigits(6);
	freq->setLineWidth(0);
	freq_layout->addWidget(freq, 0, 2);
	freq_layout->setColumnStretch(2, 3);

	// below frequency, RDS indication
	rdsLabel = new QLabel(" ");
	rdsLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	rdsLabel->setFont(bt_global::font->get(FontManager::SUBTITLE));

	// tuning control and station memories
	tuning_widget = new QStackedWidget;
	tuning_widget->insertWidget(STATION_SELECTION, getStationSelectionWidget());
	tuning_widget->insertWidget(MEMORY, getMemoryWidget());

	manual=false;

	QVBoxLayout *vbox = new QVBoxLayout(content);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->setSpacing(0);
	vbox->addWidget(radioName, 0, Qt::AlignHCenter);
	vbox->addWidget(ambDescr, 0, Qt::AlignHCenter);
	vbox->addLayout(freq_layout, 1);
	vbox->addWidget(rdsLabel, 1);
	vbox->addWidget(tuning_widget, 1 , Qt::AlignBottom);
	return content;
}

QWidget *radio::getStationSelectionWidget()
{
	QWidget *w = new QWidget;
	QGridLayout *grid = new QGridLayout(w);
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	memoBut = new BtButton;
	decBut = new BtButton;
	aumBut = new BtButton;
	autoBut = new BtButton;
	manBut = new BtButton;
	grid->addWidget(decBut, 0, 0);
	grid->addWidget(autoBut, 0, 1);
	grid->addWidget(manBut, 0, 2);
	grid->addWidget(aumBut, 0, 3);
	grid->addWidget(memoBut, 1, 1, 1, 2);

	aumBut->setImage(bt_global::skin->getImage("plus"));
	decBut->setImage(bt_global::skin->getImage("minus"));
	memoBut->setImage(bt_global::skin->getImage("memory"));
	manual_off = bt_global::skin->getImage("man_off");
	manual_on = bt_global::skin->getImage("man_on");
	auto_off = bt_global::skin->getImage("auto_off");
	auto_on = bt_global::skin->getImage("auto_on");
	manBut->setImage(manual_off);
	autoBut->setImage(auto_on);

	connect(autoBut,SIGNAL(clicked()),this,SLOT(setAuto()));
	connect(manBut,SIGNAL(clicked()),this,SLOT(setMan()));
	connect(memoBut,SIGNAL(clicked()),this,SLOT(cambiaContesto()));
	connect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	connect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));

	return w;
}

QWidget *radio::getMemoryWidget()
{
	QList<BtButton *> buttons;
	// memory numbers buttons
	for (int i = 0; i < 5; ++i)
	{
		BtButton *b = new BtButton;
		QString str = QString("num_%1").arg(i + 1);
		b->setImage(bt_global::skin->getImage(str));
		buttons << b;
		// Each button must be associated with the memory location, which go 1-5 instead of 0-4
		button_group.addButton(b, i + 1);
	}
	// cancel button
	BtButton *b = new BtButton;
	b->setImage(bt_global::skin->getImage("cancel"));
	buttons << b;
	connect(b, SIGNAL(clicked()), SLOT(ripristinaContesto()));
	connect(&button_group, SIGNAL(buttonClicked(int)), SLOT(memo(int)));

	// create widget and insert buttons into the grid
	QWidget *w = new QWidget;
	QGridLayout *grid = new QGridLayout(w);
	grid->setContentsMargins(0, 0, 0, 0);
	grid->setSpacing(0);
	int index = 0;
	for (int r = 0; r < 2; ++r)
		for (int c = 0; c < 3; ++c)
			grid->addWidget(buttons[index++], r, c);
	return w;
}

void radio::setFreq(float f)
{
	frequenza = f;
	QString fr;
	fr.sprintf("%.2f",frequenza);
	freq->display(fr);
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

void radio::setAuto()
{
	connect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	connect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	disconnect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqMan()));
	disconnect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqMan()));
	disconnect(aumBut,SIGNAL(clicked()),this,SLOT(verTas()));
	disconnect(decBut,SIGNAL(clicked()),this,SLOT(verTas()));
	aumBut->setAutoRepeat (false);
	decBut->setAutoRepeat (false);
	if (manual)
	{
		manual = false;
		manBut->setImage(manual_off, BtButton::NO_FLAG);
		autoBut->setImage(auto_on, BtButton::NO_FLAG);
	}
}

void radio::setMan()
{
	disconnect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqAuto()));
	disconnect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqAuto()));
	connect(decBut,SIGNAL(clicked()),this,SIGNAL(decFreqMan()));
	connect(aumBut,SIGNAL(clicked()),this,SIGNAL(aumFreqMan()));
	aumBut->setAutoRepeat (true);
	decBut->setAutoRepeat (true);
	connect(aumBut,SIGNAL(clicked()),this,SLOT(verTas()));
	connect(decBut,SIGNAL(clicked()),this,SLOT(verTas()));
	if (!manual)
	{
		manual = true;
		manBut->setImage(manual_on, BtButton::NO_FLAG);
		autoBut->setImage(auto_off, BtButton::NO_FLAG);
	}
}

void radio::memo(int memory)
{
	emit(memoFreq(uchar(memory)));
	ripristinaContesto();
}

void radio::cambiaContesto()
{
	state = MEMORY;
	cicBut->hide();
	tuning_widget->setCurrentIndex(state);
}

void radio::ripristinaContesto()
{
	state = STATION_SELECTION;
	cicBut->show();
	tuning_widget->setCurrentIndex(state);
}

void radio::handleClose()
{
	if (state == MEMORY)
		ripristinaContesto();
	else
		emit Closed();
}

void radio::verTas()
{
	if ((!aumBut->isDown()) && (!decBut->isDown()))
		emit (richFreq());
}
