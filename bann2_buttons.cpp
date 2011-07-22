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
#include "btmain.h" // bt_global::btmain
#include "hardware_functions.h" // maxWidth()

#include <QWidget>
#include <QLabel>
#include <QVariant> // used for setProperty
#include <QBoxLayout>
#include <QTimer>
#include <QLCDNumber>

Bann2LinkedPages::Bann2LinkedPages(QWidget *parent) : Banner(parent)
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

void BannOnOffState::initBanner(const QString &left, const QString &_center_on, const QString &_center_off, const QString &right,
	States init_state, const QString &banner_text)
{
	center_on = _center_on;
	center_off = _center_off;

	Bann2Buttons::initBanner(left, center_off, right, banner_text);
	setState(init_state);
}

void BannOnOffState::setState(States new_state)
{
	setBackgroundImage(new_state == ON ? center_on : center_off);
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

	main_layout = new QGridLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
#ifdef LAYOUT_TS_10
	main_layout->setHorizontalSpacing(5);
	main_layout->setVerticalSpacing(0);
#else
	main_layout->setHorizontalSpacing(10);
	main_layout->setVerticalSpacing(5);
#endif
	main_layout->addWidget(left_button, 0, 0, Qt::AlignLeft);
	main_layout->setColumnStretch(0, 1);
	// central widget added in initBanner()
	main_layout->setColumnStretch(1, 2);
	main_layout->addWidget(right_button, 0, 2, Qt::AlignRight);
	main_layout->setColumnStretch(2, 1);
	main_layout->addWidget(description, 1, 0, 1, 3);

	connect(right_button, SIGNAL(clicked()), SIGNAL(rightClicked()));
	connect(right_button, SIGNAL(pressed()), SIGNAL(rightPressed()));
	connect(right_button, SIGNAL(released()), SIGNAL(rightReleased()));

	connect(left_button, SIGNAL(clicked()), SIGNAL(leftClicked()));
	connect(left_button, SIGNAL(pressed()), SIGNAL(leftPressed()));
	connect(left_button, SIGNAL(released()), SIGNAL(leftReleased()));
}

void Bann2Buttons::initBanner(const QString &left, const QString &right, const QString &banner_text,
	FontManager::Type text_font, const QString &banner_description, FontManager::Type description_font)
{
	center_label = new ScrollingLabel;
	main_layout->addWidget(center_label, 0, 1, Qt::AlignCenter);

	initButton(left_button, left);
	initButton(right_button, right);
	if (right.isEmpty())
	{
		main_layout->setColumnStretch(2, 0);
	}
	center_label->setScrollingText(banner_text);
	QFont central_font = bt_global::font->get(text_font);

	center_label->setFont(central_font);

	initLabel(description, banner_description, bt_global::font->get(description_font));
}

void Bann2Buttons::initBanner(const QString &left, const QString &center, const QString &right, const QString &descr,
			      FontManager::Type description_font)
{
	center_icon = new TextOnImageLabel;

	main_layout->addWidget(center_icon, 0, 1);

	initButton(left_button, left);
	initButton(right_button, right);
	center_icon->setBackgroundImage(center);

	initLabel(description, descr, bt_global::font->get(description_font));
}

void Bann2Buttons::setTextAlignment(Qt::Alignment align)
{
	if (center_label)
	{
		// Reset the alignment of the cell that contains the label if the requested
		// label alignment not contains AlignHCenter.
		if ((align & Qt::AlignHCenter) == 0)
			main_layout->itemAtPosition(0, 1)->setAlignment(0);

		center_label->setAlignment(align);
	}
}

void Bann2Buttons::setCentralText(const QString &t)
{
	if (center_icon)
		center_icon->setInternalText(t);
	else
		center_label->setScrollingText(t);
}

void Bann2Buttons::setDescriptionText(const QString &t)
{
	description->setScrollingText(t);
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


BannOpenClose::BannOpenClose(QWidget *parent) : Bann2Buttons(parent)
{
}


void BannOpenClose::initBanner(QString _left, QString _center, QString _right, QString lr_alternate,
	States starting_state, QString banner_text)
{
	left =_left;
	center = _center;
	right = _right;
	alternate = lr_alternate;
	Bann2Buttons::initBanner(left, center, right, banner_text);
	setState(starting_state);
}

void BannOpenClose::setState(States new_state)
{
	switch (new_state)
	{
	case STOP:
		setBackgroundImage(center);
		left_button->setImage(left);
		right_button->setImage(right);
		break;
	case OPENING:
		setBackgroundImage(getBostikName(center, "o"));
		right_button->setImage(alternate);
		left_button->setImage(left);
		break;
	case CLOSING:
		setBackgroundImage(getBostikName(center, "c"));
		right_button->setImage(right);
		left_button->setImage(alternate);
		break;
	}
}


BannOnOff2Labels::BannOnOff2Labels(QWidget *parent) : Banner(parent)
{
	left_button = new BtButton;
	right_button = new BtButton;
	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	center_icon = new QLabel;
	center_text = createTextLabel(Qt::AlignCenter, bt_global::font->get(FontManager::BANNERTEXT));

	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->addWidget(left_button, 0, Qt::AlignLeft);
	hbox->addWidget(center_text, 1, Qt::AlignCenter);
	hbox->addWidget(center_icon, 0, Qt::AlignCenter);
	hbox->addWidget(right_button, 0, Qt::AlignRight);

	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->addLayout(hbox);
	main_layout->addWidget(text);

#ifdef LAYOUT_TS_3_5
	main_layout->setSpacing(5);
	hbox->setSpacing(10);
#else
	main_layout->setSpacing(0);
	hbox->setSpacing(0);
#endif
}

void BannOnOff2Labels::initBanner(const QString &left, const QString &_center_on,
				  const QString &_center_off, const QString &right,
				  States init_state, const QString &banner_text,
				  const QString &second_text)
{
	left_button->setImage(left);
	center_on = _center_on;
	center_off = _center_off;
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
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center_on));
		center_icon->setMinimumSize(center_icon->pixmap()->size());
		break;
	case OFF:
		center_icon->setPixmap(*bt_global::icons_cache.getIcon(center_off));
		center_icon->setMinimumSize(center_icon->pixmap()->size());
		break;
	}
}


Bann2CentralButtons::Bann2CentralButtons(bool spaced_buttons)
{
	center_left = new BtButton;
	center_right = new BtButton;

	text = createTextLabel(Qt::AlignHCenter, bt_global::font->get(FontManager::BANNERDESCRIPTION));
	QGridLayout *l = new QGridLayout;
	l->setContentsMargins(0, 0, 0, 0);
#ifdef LAYOUT_TS_10
	l->setSpacing(spaced_buttons ? 5 : 0);
#else
	l->setSpacing(spaced_buttons ? 10 : 0);
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

BannLCDRange::BannLCDRange(QWidget *parent) : Banner(parent)
{
	control_timer = new QTimer(this);

	minus = new BtButton(bt_global::skin->getImage("minus"));
	minus->setAutoRepeat(true);
	plus = new BtButton(bt_global::skin->getImage("plus"));
	plus->setAutoRepeat(true);

	lcd = new QLCDNumber;
	lcd->setMinimumHeight(50);
	lcd->setSegmentStyle(QLCDNumber::Flat);
	lcd->setFrameShape(QFrame::NoFrame);

	min = 0;
	max = 100;

	connect(control_timer, SIGNAL(timeout()), SLOT(emitValueChanged()));
	connect(minus, SIGNAL(clicked()), SLOT(minusClicked()));
	connect(plus, SIGNAL(clicked()), SLOT(plusClicked()));

#ifdef LAYOUT_TS_3_5
	// Empirical alignment to force the banner to occupy the whole width, with the
	// lcd aligned in the center. We cannot use layouts thanks to the old api
	// for the QLCDNumber (see the comment on RadioInfo).
	lcd->setParent(this);
	lcd->setGeometry(10, 0, maxWidth() - 40, minus->height() + 10);
	minus->setParent(this);
	minus->move(0, 5);

	plus->setParent(this);
	plus->move(maxWidth() - plus->width() - 10, 5);

#else
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);

	hbox->addWidget(minus, 0, Qt::AlignLeft);

	hbox->addWidget(lcd, 1, Qt::AlignCenter);
	hbox->addWidget(plus, 0, Qt::AlignRight);
#endif
}

QSize BannLCDRange::sizeHint() const
{
#ifdef LAYOUT_TS_3_5
	return QSize(maxWidth() - 10, minus->height() + 10);
#else
	return layout()->sizeHint();
#endif
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


BannOnTray::BannOnTray(const QString &label, const QString &icon_on, const QString &icon_off,
	const QString &tray_icon, TrayBar::ButtonId tray_id, bool status, int _item_id)
{
	initBanner(icon_off, QString(), label);
	setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	left_button->setOffImage(bt_global::skin->getImage(icon_off));
	left_button->setOnImage(bt_global::skin->getImage(icon_on));
	left_button->setStatus(status);
	connect(left_button, SIGNAL(clicked()), SLOT(toggleActivation()));

	tray_button = new BtButton(bt_global::skin->getImage(tray_icon));
	bt_global::btmain->trayBar()->addButton(tray_button, tray_id);

	item_id = _item_id;
}

void BannOnTray::toggleActivation()
{
	left_button->setStatus(!left_button->getStatus());
	if (item_id != -1)
		setCfgValue("enable", left_button->getStatus(), item_id);
	updateStatus();
}

void BannOnTray::updateStatus()
{
	tray_button->setVisible(left_button->getStatus() == StateButton::ON);
}
