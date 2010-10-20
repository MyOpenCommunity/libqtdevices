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

	BtButton *ok = new BtButton;
	BtButton *canc = new BtButton;
	BtButton *digits[10];

	QButtonGroup *buttons = new QButtonGroup;

	for (int i = 0; i < 10; ++i)
	{
		digits[i] = new BtButton;
		digits[i]->setImage(bt_global::skin->getImage("num_" + QString::number(i)));
		buttons->addButton(digits[i], i);
	}

	digitLabel = new QLabel;
	msgLabel = new QLabel;
	warningLabel = new QLabel(tr("Wrong password"));
	warningLabel->hide();

	ok->setImage(bt_global::skin->getImage("ok"));
	canc->setImage(bt_global::skin->getImage("cancel"));

	mode = CLEAN;

	msgLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	msgLabel->setFont(bt_global::font->get(FontManager::TEXT));
	msgLabel->setText(tr("PASSWORD:"));

	digitLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	digitLabel->setFont(bt_global::font->get(FontManager::TEXT));

	connect(buttons, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
	connect(canc, SIGNAL(clicked()), SLOT(deleteChar()));
	connect(ok, SIGNAL(clicked()), SIGNAL(accept()));

	// digits, ok, cancel buttons
	QGridLayout *k = new QGridLayout;
#ifdef LAYOUT_TS_10
	k->setAlignment(Qt::AlignHCenter);
	k->setSpacing(20);
#endif
	k->setContentsMargins(0, 0, 0, 0);

	for (int i = 0; i < 9; ++i)
		k->addWidget(digits[i + 1], i / 3, i % 3);

	k->addWidget(canc, 3, 0);
	k->addWidget(digits[0], 3, 1);
	k->addWidget(ok, 3, 2);

	// bottom labels
	QHBoxLayout *p = new QHBoxLayout;
	p->setContentsMargins(0, 0, 0, 0);
	p->setSpacing(10);

	p->addWidget(msgLabel, 1, Qt::AlignRight);
	p->addWidget(digitLabel, 1, Qt::AlignLeft);

	// top layout
	topLayout = new QGridLayout(top_widget);
	topLayout->setContentsMargins(0, 0, 0, 10);
	// avoid expansion of keys
	topLayout->setColumnStretch(0, 1);
	topLayout->setColumnStretch(2, 1);

#ifdef LAYOUT_TS_3_5
	topLayout->setSpacing(0);
#else
	topLayout->setSpacing(15);
#endif

	// when modifying this, modify insertLayout below
	topLayout->addLayout(k, 0, 1);
	topLayout->addLayout(p, 2, 1);
	topLayout->addWidget(warningLabel, 3, 1);
	topLayout->setRowStretch(3, 1);

	updateText();
}

void Keypad::insertLayout(QLayout *l)
{
	topLayout->addLayout(l, 1, 1);
}

void Keypad::showWrongPassword(bool is_visible)
{
	warningLabel->setVisible(is_visible);
}

void Keypad::setMessage(const QString &message)
{
	msgLabel->setText(message);
}

void Keypad::updateText()
{
	if (mode == CLEAN)
		digitLabel->setText(text);
	else
		digitLabel->setText(QString(text.length(),'*'));
	// always set a text on the label, otherwise the sizeHint() height changes
	if (text.length() == 0)
		digitLabel->setText(" ");
}

void Keypad::buttonClicked(int number)
{
	if (text.length() < 5)
		text += QString::number(number);
	updateText();
	warningLabel->hide();
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


// KeypadWithState implementation

#ifdef LAYOUT_TS_3_5

KeypadWithState::KeypadWithState(int s[8])
{
	QHBoxLayout *l = new QHBoxLayout;
	l->setContentsMargins(5, 5, 5, 5);
	l->setSpacing(2);

	QFont aFont = bt_global::font->get(FontManager::TEXT);

	for (int i = 0; i < 8; i++)
	{
		QLabel *state = new QLabel;

		state->setFont(aFont);
		state->setAlignment(Qt::AlignCenter);

		if (s[i] == -1)
		{
			state->setText("-");
		}
		else
		{
			state->setProperty("ActiveState", bool(s[i]));
			state->setText(QString::number(i + 1));
		}

		l->addWidget(state);
	}

	insertLayout(l);
}

#else

KeypadWithState::KeypadWithState(int s[8])
{
	QHBoxLayout *l = new QHBoxLayout;
	l->setContentsMargins(5, 5, 5, 5);
	l->setSpacing(5);
	l->setAlignment(Qt::AlignHCenter);

	for (int i = 0; i < 8; i++)
	{
		if (s[i] == -1)
			continue;

		QLabel *state = new QLabel;
		QString icon = bt_global::skin->getImage("small_" + QString::number(i + 1));

		if (s[i])
			icon = getBostikName(icon, "on");
		else
			icon = getBostikName(icon, "off");
		state->setPixmap(*bt_global::icons_cache.getIcon(icon));

		l->addWidget(state);
	}

	insertLayout(l);
}

#endif


// KeypadWindow implementation

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
