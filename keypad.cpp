#include "keypad.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h"
#include "btbutton.h"

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>


// Keypad implementation

Keypad::Keypad()
{
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
	QLabel *pwdLabel = new QLabel;

	ok->setImage(bt_global::skin->getImage("ok"));
	canc->setImage(bt_global::skin->getImage("cancel"));

	mode = CLEAN;

	pwdLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	pwdLabel->setFont(bt_global::font->get(FontManager::TEXT));
	pwdLabel->setText(tr("PASSWORD:"));

	digitLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	digitLabel->setFont(bt_global::font->get(FontManager::TEXT));

	connect(buttons, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
	connect(canc, SIGNAL(clicked()), SLOT(deleteChar()));
	connect(ok, SIGNAL(clicked()), SIGNAL(Closed()));

	// digits, ok, cancel buttons
	QGridLayout *k = new QGridLayout;
#ifdef LAYOUT_TOUCHX
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
	p->setSpacing(0);

	p->addWidget(pwdLabel, 1);
	p->addWidget(digitLabel, 1);

	// top layout
	topLayout = new QVBoxLayout(this);
	topLayout->setContentsMargins(0, 0, 0, 10);
#ifdef LAYOUT_BTOUCH
	topLayout->setSpacing(0);
#else
	topLayout->setSpacing(15);
#endif

	// when modifying this, modify insertLayout below
	topLayout->addLayout(k);
	topLayout->addLayout(p);

	updateText();
}

void Keypad::insertLayout(QLayout *l)
{
	topLayout->insertLayout(1, l);
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


// KeypadWindow implementation

KeypadWindow::KeypadWindow(Keypad::Type type)
{
	keypad = new Keypad;
	keypad->setMode(type);

	connect(keypad, SIGNAL(Closed()), SIGNAL(Closed()));

	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

#ifdef LAYOUT_BTOUCH
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
