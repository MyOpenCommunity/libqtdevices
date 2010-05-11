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


#include "bann2_buttons.h"
#include "state_button.h"
#include "icondispatcher.h" // icons_cache
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName
#include "labels.h" // TextOnImageLabel

#include <QWidget>
#include <QLabel>
#include <QVariant> // used for setProperty
#include <QBoxLayout>
#include <QTimer>
#include <QLCDNumber>

Bann2LinkedPages::Bann2LinkedPages(QWidget *parent) :
	BannerNew(parent)
{
}

void Bann2LinkedPages::connectLeftButton(Page *p)
{
	connectButtonToPage(left_button, p);
}

void Bann2LinkedPages::connectRightButton(Page *p)
{
	connectButtonToPage(right_button, p);
}


BannOnOffState::BannOnOffState(QWidget *parent) :
	Bann2Buttons(parent)
{
}

void BannOnOffState::initBanner(const QString &left, const QString &_center, const QString &right,
	States init_state, const QString &banner_text)
{
	center = _center;
	Bann2Buttons::initBanner(left, center, right, banner_text);
	setState(init_state);
}

void BannOnOffState::setState(States new_state)
{
	setBackgroundImage(getBostikName(center, new_state == ON ? "on" : "off"));
}


Bann2Buttons::Bann2Buttons(QWidget *parent) :
	Bann2LinkedPages(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;

	createBanner();
}

void Bann2Buttons::createBanner()
{
	center_icon = 0;
	center_label = 0;
	description = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));

	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addWidget(left_button, 0, 0, Qt::AlignLeft);
	l->setColumnStretch(0, 1);
	// central widget added in initBanner()
	l->setColumnStretch(1, 2);
	l->addWidget(right_button, 0, 2, Qt::AlignRight);
	l->setColumnStretch(2, 1);
	l->addWidget(description, 1, 0, 1, 3);

	connect(right_button, SIGNAL(clicked()), SIGNAL(rightClicked()));
	connect(left_button, SIGNAL(clicked()), SIGNAL(leftClicked()));
}

void Bann2Buttons::initBanner(const QString &left, const QString &right, const QString &banner_text,
	FontManager::Type text_font, const QString &banner_description, FontManager::Type description_font)
{
	center_label = new QLabel;
	static_cast<QGridLayout*>(layout())->addWidget(center_label, 0, 1, Qt::AlignCenter);

	initButton(left_button, left);
	initButton(right_button, right);
	if (right.isEmpty())
	{
		QGridLayout *l = static_cast<QGridLayout*>(layout());
		l->setColumnStretch(2, 0);
	}
	center_label->setText(banner_text);
	QFont central_font = bt_global::font->get(text_font);

	center_label->setFont(central_font);

	initLabel(description, banner_description, bt_global::font->get(description_font));
}

void Bann2Buttons::initBanner(const QString &left, const QString &center, const QString &right, const QString &descr,
			      FontManager::Type description_font)
{
	center_icon = new TextOnImageLabel;
	static_cast<QGridLayout*>(layout())->addWidget(center_icon, 0, 1);

	initButton(left_button, left);
	initButton(right_button, right);
	center_icon->setBackgroundImage(center);

	initLabel(description, descr, bt_global::font->get(description_font));
}

void Bann2Buttons::setTextAlignment(Qt::Alignment align)
{
	if (center_label)
		center_label->setAlignment(align);
}

void Bann2Buttons::setCentralText(const QString &t)
{
	if (center_icon)
		center_icon->setInternalText(t);
	else
		center_label->setText(t);
}

void Bann2Buttons::setDescriptionText(const QString &t)
{
	description->setText(t);
}

void Bann2Buttons::setBackgroundImage(const QString &path)
{
	Q_ASSERT_X(center_icon, "Bann2Buttons::setBackgroundImage", "Bann2Button created without a background image");
	center_icon->setBackgroundImage(path);
}


Bann2StateButtons::Bann2StateButtons(QWidget *parent) :
	Bann2Buttons(parent, static_cast<StateButton *>(0))
{
	left_button = static_cast<StateButton *>(Bann2Buttons::left_button);
	right_button = static_cast<StateButton *>(Bann2Buttons::right_button);
}


BannOpenClose::BannOpenClose(QWidget *parent) :
	BannerNew(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(left_button, 0, Qt::AlignLeft);
	hbox->addWidget(center_icon, 1, Qt::AlignHCenter);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannOpenClose::initBanner(QString left, QString center, QString right, QString lr_alternate,
	States starting_state, QString banner_text)
{
	loadIcons(left, center, right, lr_alternate);
	setState(starting_state);
	text->setText(banner_text);
}

void BannOpenClose::loadIcons(QString _left, QString _center, QString _right, QString _alternate)
{
	right = _right;
	left =_left;
	center = _center;
	alternate = _alternate;
}

void BannOpenClose::setState(States new_state)
{
	switch (new_state)
	{
	case STOP:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
		left_button->setImage(left);
		right_button->setImage(right);
		break;
	case OPENING:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "o")));
		right_button->setImage(alternate);
		left_button->setImage(left);
		break;
	case CLOSING:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "c")));
		right_button->setImage(right);
		left_button->setImage(alternate);
		break;
	}
}


BannOnOff2Labels::BannOnOff2Labels(QWidget *parent) :
	BannerNew(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;
	center_text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERTEXT));

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(0);
	hbox->addWidget(left_button, 0, Qt::AlignLeft);
	hbox->addWidget(center_text, 1, Qt::AlignCenter);
	hbox->addWidget(center_icon, 0, Qt::AlignCenter);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->addLayout(hbox);
	l->addWidget(text);
}

void BannOnOff2Labels::initBanner(const QString &left, const QString &_center, const QString &right,
		States init_state, const QString &banner_text, const QString &second_text)
{
	left_button->setImage(left);
	center = _center;
	right_button->setImage(right);

	text->setText(banner_text);
	center_text->setText(second_text);
	setState(init_state);
}

void BannOnOff2Labels::setCentralText(const QString &str)
{
	center_text->setText(str);
}

void BannOnOff2Labels::setCentralTextSecondaryColor(bool secondary)
{
	center_text->setProperty("SecondFgColor", secondary);
}

void BannOnOff2Labels::setState(States new_state)
{
	switch (new_state)
	{
	case ON:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center));
		break;
	case OFF:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(getBostikName(center, "off")));
		break;
	}
}


Bann2CentralButtons::Bann2CentralButtons(bool spaced_buttons) : BannerNew(0)
{
	center_left = new BtButton;
	center_right = new BtButton;

	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	QGridLayout *l = new QGridLayout;
	l->setContentsMargins(0, 0, 0, 0);
#ifdef LAYOUT_TOUCHX
	l->setSpacing(spaced_buttons ? 5 : 0);
#else
	l->setSpacing(0);
#endif
	l->setColumnStretch(0, 1);
	l->addWidget(center_left, 0, 1, Qt::AlignTop);
	l->addWidget(center_right, 0, 2, Qt::AlignTop);
	l->setColumnStretch(3, 1);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->addLayout(l);
	layout->addWidget(text);
}

void Bann2CentralButtons::initBanner(const QString &left, const QString &right, const QString &banner_text)
{
	center_left->setImage(left);
	center_right->setImage(right);
	if (banner_text.isEmpty())
	{
		// just to avoid surprises...
		text->disconnect();
		text->deleteLater();
	}
	else
		text->setText(banner_text);
}

#define INPUT_INTERVAL 500

BannLCDRange::BannLCDRange(QWidget *parent) :
	BannerNew(parent), control_timer(new QTimer(this)), lcd(new QLCDNumber),
	minus(new BtButton), plus(new BtButton),
	min(0), max(100)
{
	initBanner();

	connect(control_timer, SIGNAL(timeout()), SLOT(emitValueChanged()));
	connect(minus, SIGNAL(clicked()), SLOT(minusClicked()));
	connect(plus, SIGNAL(clicked()), SLOT(plusClicked()));

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);

	hbox->addWidget(minus, 0, Qt::AlignLeft);
	hbox->addWidget(lcd, 1, Qt::AlignCenter);
	hbox->addWidget(plus, 0, Qt::AlignRight);

	setLayout(hbox);
}

void BannLCDRange::setRange(int minimum, int maximum)
{
	min = minimum;
	max = maximum;
}

void BannLCDRange::setValue(int value)
{
	int v = 0;

	if (value < min)
		v = min;
	else if (value > max)
		v = max;
	else
		v = value;

	lcd->display(v);
}

void BannLCDRange::setNumDigits(int n)
{
	lcd->setNumDigits(n);
}

int BannLCDRange::value() const
{
	return lcd->intValue();
}

void BannLCDRange::initBanner()
{
	control_timer->setInterval(INPUT_INTERVAL);

	minus->setImage(bt_global::skin->getImage("minus"));
	minus->setAutoRepeat(true);

	plus->setImage(bt_global::skin->getImage("plus"));
	plus->setAutoRepeat(true);

	lcd->setMinimumHeight(50);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setFrameShape(QFrame::NoFrame);
}

void BannLCDRange::startTimer()
{
	if (control_timer->isActive())
		control_timer->stop();

	control_timer->start();
}

void BannLCDRange::plusClicked()
{
	setValue(value() + 1);
	startTimer();
}

void BannLCDRange::minusClicked()
{
	setValue(value() - 1);
	startTimer();
}

void BannLCDRange::emitValueChanged()
{
	control_timer->stop();

	emit valueChanged(lcd->intValue());
}
