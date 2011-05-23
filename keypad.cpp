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


#include "keypad.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h"
#include "icondispatcher.h"
#include "btbutton.h"
#include "navigation_bar.h"
#include "generic_functions.h" // getBostikName
#include "hardware_functions.h" // maxWidth()

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>


// Keypad implementation

Keypad::Keypad(bool back_button)
{
	// TODO extract the keypad to a widget and use it in both page and window
#ifdef LAYOUT_TS_10
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	buildPage(new QWidget, nav_bar);
	QWidget *top_widget = page_content;
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	if (!back_button)
	{
		nav_bar->hide();
		nav_bar->deleteLater();
	}
#else
	QWidget *top_widget = this;
#endif

	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));
	BtButton *canc = new BtButton(bt_global::skin->getImage("cancel"));
	BtButton *digits[10];

	QButtonGroup *buttons = new QButtonGroup;

	for (int i = 0; i < 10; ++i)
	{
		digits[i] = new BtButton(bt_global::skin->getImage("num_" + QString::number(i)));
		buttons->addButton(digits[i], i);
	}

	digit_label = new QLabel;
	msg_label = new QLabel;
	warning_label = new QLabel(tr("Wrong password"));
	warning_label->hide();

	mode = CLEAN;

	msg_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	msg_label->setFont(bt_global::font->get(FontManager::TEXT));
	msg_label->setText(tr("PASSWORD:"));

	digit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	digit_label->setFont(bt_global::font->get(FontManager::TEXT));

	connect(buttons, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
	connect(canc, SIGNAL(clicked()), SLOT(deleteChar()));
	connect(ok, SIGNAL(clicked()), SIGNAL(accept()));

	// digits, ok, cancel buttons
	QGridLayout *k = new QGridLayout;
	k->setContentsMargins(0, 0, 0, 0);

#ifdef LAYOUT_TS_3_5
	k->setHorizontalSpacing(25);
	k->setVerticalSpacing(10);
#else
	k->setAlignment(Qt::AlignHCenter);
	k->setSpacing(20);
#endif

	for (int i = 0; i < 9; ++i)
		k->addWidget(digits[i + 1], i / 3, i % 3);

	k->addWidget(canc, 3, 0);
	k->addWidget(digits[0], 3, 1);
	k->addWidget(ok, 3, 2);

	// bottom labels
	QHBoxLayout *p = new QHBoxLayout;
#ifdef LAYOUT_TS_3_5
	p->setContentsMargins(0, 10, 0, 0);
#else
	p->setContentsMargins(0, 0, 0, 0);
#endif

	p->setSpacing(10);

	p->addWidget(msg_label, 1, Qt::AlignRight);
	p->addWidget(digit_label, 1, Qt::AlignLeft);

	// top layout
	top_layout = new QGridLayout(top_widget);

	// avoid expansion of keys
	top_layout->setColumnStretch(0, 1);
	top_layout->setColumnStretch(2, 1);

#ifdef LAYOUT_TS_3_5
	top_layout->setSpacing(0);
	top_layout->setContentsMargins(5, 5, 5, 10);
#else
	top_layout->setSpacing(15);
	top_layout->setContentsMargins(0, 0, 0, 10);
#endif

	// when modifying this, modify insertLayout below
	top_layout->addLayout(k, 0, 1);
	top_layout->addLayout(p, 2, 1);
	top_layout->addWidget(warning_label, 3, 1);
	top_layout->setRowStretch(3, 1);

	updateText();
}

void Keypad::insertLayout(QLayout *l)
{
	top_layout->addLayout(l, 1, 1);
}

void Keypad::showWrongPassword(bool is_visible)
{
	warning_label->setVisible(is_visible);
}

void Keypad::setMessage(const QString &message)
{
	msg_label->setText(message);
}

void Keypad::updateText()
{
	if (mode == CLEAN)
		digit_label->setText(text);
	else
		digit_label->setText(QString(text.length(),'*'));
	// always set a text on the label, otherwise the sizeHint() height changes
	if (text.length() == 0)
		digit_label->setText(" ");
}

void Keypad::buttonClicked(int number)
{
	if (text.length() < 5)
		text += QString::number(number);
	updateText();
	warning_label->hide();
}

void Keypad::deleteChar()
{
	if (text.length() > 0)
	{
		text.chop(1);
		updateText();
	}
	else
	{
		emit Closed();
	}
}

void Keypad::setMode(Type t)
{
	mode = t;
	updateText();
}

QString Keypad::getText()
{
	return text;
}

void Keypad::resetText()
{
	text = "";
	updateText();
}


KeypadWithState::KeypadWithState(const QList<int> &s)
{
	states_layout = new QHBoxLayout;
#ifdef LAYOUT_TS_3_5
	states_layout->setContentsMargins(5, 15, 5, 5);
	states_layout->setSpacing(0);
#else
	states_layout->setContentsMargins(5, 5, 5, 5);
	states_layout->setSpacing(5);
#endif

	states_layout->setAlignment(Qt::AlignHCenter);
	insertLayout(states_layout);

	if (!s.isEmpty())
		setStates(s);
}

void KeypadWithState::setStates(const QList<int> &s)
{
	foreach(QLabel *state, states)
	{
		if (state)
		{
			states_layout->removeWidget(state);
			state->deleteLater();
		}
	}

	states.clear();
	drawStates(s);
}

void KeypadWithState::drawStates(const QList<int> &s)
{
#ifdef LAYOUT_TS_3_5
	QFont state_font = bt_global::font->get(FontManager::TEXT);

	// We want the label bigger than the text, as bigger as we can.
	int left, top, right, bottom;
	layout()->getContentsMargins(&left, &top, &right, &bottom);
	int max_width = maxWidth() - left - right;
	states_layout->getContentsMargins(&left, &top, &right, &bottom);
	max_width = max_width - left - right;
	int state_width = (max_width / s.size()) - states_layout->spacing();

	for (int i = 0; i < s.size(); ++i)
	{
		QLabel *state = new QLabel;
		state->setFont(state_font);
		state->setAlignment(Qt::AlignCenter);

		if (s[i] == -1)
			state->setText("-");
		else
		{
			state->setProperty("ActiveState", bool(s[i]));
			state->setText(QString::number(i + 1));
		}

		state->setFixedSize(state_width, state_width);
		states_layout->addWidget(state);
		states.append(state);
	}
#else
	for (int i = 0; i < s.size(); ++i)
	{
		QLabel *state = 0;

		if (s[i] != -1)
		{
			state = new QLabel;
			QString icon = bt_global::skin->getImage("small_" + QString::number(i + 1));

			icon = getBostikName(icon, s[i] ? "on" : "off");
			state->setPixmap(*bt_global::icons_cache.getIcon(icon));

			states_layout->addWidget(state);
		}
		states.append(state);
	}
#endif
}


KeypadWindow::KeypadWindow(Keypad::Type type)
{
	keypad = new Keypad(false);
	keypad->setMode(type);

	connect(keypad, SIGNAL(accept()), SIGNAL(Closed()));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

#ifdef LAYOUT_TS_3_5
	l->addWidget(keypad, 1);
#else
	l->addWidget(keypad, 1, Qt::AlignCenter);
#endif

	keypad->show();
}

QString KeypadWindow::getText()
{
	return keypad->getText();
}

void KeypadWindow::resetText()
{
	keypad->resetText();
}
